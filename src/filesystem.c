#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _XOPEN_SOURCE_EXTENDED 1
#include <stdlib.h>

#include <fuse.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <gtk/gtk.h>
#include "params.h"
#include "validation.h"
#include "email.h"
#include "utils.h"
#include "filesystem.h"
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

struct nfs_config conf;

static void nfs_fullpath(char fpath[PATH_MAX], const char *path) {
    strcpy(fpath, NFS_DATA->rootdir);
    strncat(fpath, path, PATH_MAX); // ridiculously long paths will break here
}

static int nfs_getattr(const char *path, struct stat *stbuf) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = lstat(fpath, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_access(const char *path, int mask) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = access(fpath, mask);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_readlink(const char *path, char *buf, size_t size) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = readlink(fpath, buf, size - 1);
    if (res == -1)
        return -errno;

    buf[res] = '\0';
    return 0;
}


static int nfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    dp = opendir(fpath);
    //dp = (DIR *) (uintptr_t) fi->fh;

    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int nfs_mknod(const char *path, mode_t mode, dev_t rdev) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    /* On Linux this could just be 'mknod(path, mode, rdev)' but this
       is more portable */
    if (S_ISREG(mode)) {
        res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    } else if (S_ISFIFO(mode))
        res = mkfifo(fpath, mode);
    else
        res = mknod(fpath, mode, rdev);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_mkdir(const char *path, mode_t mode) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = mkdir(fpath, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_unlink(const char *path) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = unlink(fpath);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_rmdir(const char *path) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = rmdir(fpath);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_symlink(const char *path, const char *link) {
    int res;

    char flink[PATH_MAX];
    nfs_fullpath(flink, link);

    res = symlink(path, flink);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_rename(const char *path, const char *newpath) {
    int res;

    char fpath[PATH_MAX];
    char fnewpath[PATH_MAX];

    nfs_fullpath(fpath, path);
    nfs_fullpath(fnewpath, newpath);

    res = rename(fpath, fnewpath);

    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_link(const char *path, const char *newpath) {
    int res;
    char fpath[PATH_MAX];
    char fnewpath[PATH_MAX];

    nfs_fullpath(fpath, path);
    nfs_fullpath(fnewpath, newpath);

    res = link(fpath, fnewpath);

    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_chmod(const char *path, mode_t mode) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = chmod(fpath, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_chown(const char *path, uid_t uid, gid_t gid) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = lchown(fpath, uid, gid);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_truncate(const char *path, off_t size) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = truncate(fpath, size);
    if (res == -1)
        return -errno;

    return 0;
}

#ifdef HAVE_UTIMENSAT
static int nfs_utimens(const char *path, const struct timespec ts[2]) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    /* don't use utime/utimes since they follow symlinks */
    res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
    if (res == -1)
        return -errno;

    return 0;
}
#endif

static int nfs_open(const char *path, struct fuse_file_info *fi) {
    int res;

    char code[HASH_CODE_SIZE + 1]; // + 1 because of '\0'
    generate_rand_alphanumeric_string(HASH_CODE_SIZE, code);

    char email[BUFFER_SIZE];
    get_user_email(conf.credentials_path,email);
    send_confirmation_code(code,email);
    int authorized = validate(code,email);
    if(!authorized){
        new_error_window();
        return (-EACCES);
    }

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = open(fpath, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

static int nfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    (void) fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int nfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    (void) fi;
    fd = open(fpath, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int nfs_statfs(const char *path, struct statvfs *stbuf) {
    int res;

    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    res = statvfs(fpath, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int nfs_release(const char *path, struct fuse_file_info *fi) {
    /* Just a stub.	 This method is optional and can safely be left
       unimplemented */

    (void) path;
    (void) fi;
    return 0;
}

static int nfs_fsync(const char *path, int isdatasync, struct fuse_file_info *fi) {
    /* Just a stub.	 This method is optional and can safely be left
       unimplemented */

    (void) path;
    (void) isdatasync;
    (void) fi;
    return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int nfs_fallocate(const char *path, int mode, off_t offset, off_t length, struct fuse_file_info *fi) {
    int fd;
    int res;

    (void) fi;

    if (mode)
        return -EOPNOTSUPP;

    fd = open(path, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = -posix_fallocate(fd, offset, length);

    close(fd);
    return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int nfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    int res = lsetxattr(fpath, name, value, size, flags);
    if (res == -1)
        return -errno;
    return 0;
}

static int nfs_getxattr(const char *path, const char *name, char *value, size_t size) {
    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    int res = lgetxattr(fpath, name, value, size);
    if (res == -1)
        return -errno;
    return res;
}

static int nfs_listxattr(const char *path, char *list, size_t size) {
    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    int res = llistxattr(fpath, list, size);
    if (res == -1)
        return -errno;
    return res;
}

static int nfs_removexattr(const char *path, const char *name) {
    char fpath[PATH_MAX];
    nfs_fullpath(fpath, path);

    int res = lremovexattr(fpath, name);
    if (res == -1)
        return -errno;
    return 0;
}
#endif /* HAVE_SETXATTR */

static int nfs_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs)
{
    switch (key) {
        case KEY_VERSION:
            // See which version of fuse we're running
            fprintf(stderr, "Fuse library version %d.%d\n", FUSE_MAJOR_VERSION, FUSE_MINOR_VERSION);
            fprintf(stderr, "Authentication Mounter version 1.0\n");
            exit(0);
            break;
        case KEY_HELP:
            fprintf(stderr, "usage: authmounter [FUSE OPTIONS] [AUTHMOUNTER OPTIONS] MOUNTPOINT\n");
            fprintf(stderr, "Mount a filesystem in MOUNTPOINT where sensitive operations are restricted with a 2-way authentication system\n");
            fprintf(stderr, "Fuse options use the '-o' flag\n");
            fprintf(stderr, "\nauthmounter options:\n");
            fprintf(stderr, "\t -c, --credentials \t Path to directory that contains credencials. Must be list in format «name:email». Default: Specified by input.\n");
            fprintf(stderr, "\t -r, --root \t\t Root directory of the mount. Default: System root.\n");
            fprintf(stderr, "Example: authmounter -o nonempty -r rootDir/ -c credentials.txt mountDir\n");
            exit(0);
    }
    return 1;
}

static struct fuse_opt nfs_opts[] = {
    FUSE_OPT_KEY("-v",             KEY_VERSION),
    FUSE_OPT_KEY("--version",      KEY_VERSION),
    FUSE_OPT_KEY("-h",             KEY_HELP),
    FUSE_OPT_KEY("--help",         KEY_HELP),
    NFS_OPT("-r %s",              root_dir, 0),
    NFS_OPT("--root %s",          root_dir, 0),
    NFS_OPT("-c %s",              credentials_path, 0),
    NFS_OPT("--credentials %s",   credentials_path, 0),
    FUSE_OPT_END
};

void nfs_validate_non_root_usage() {
    if ((getuid() == 0) || (geteuid() == 0)) {
        fprintf(stderr, "For security reasons, it's not allowed to execute this as root.\n");
        exit(1);
    }
}

void nfs_validate_options(struct nfs_config* conf) {
    // No root directory specified? By default, system's root
    if (conf->root_dir == NULL) {
        printf("No root specified. Assuming \"/\"");
        conf->root_dir = "/";
    }

    // Credentials file specifies who to send 2-factor authentication email to.
    // If none specified, ask for email with a prompt and save in file
    if (conf->credentials_path == NULL) {
        printf("Input your email:");
        char line[250];
        char* lpointer = line; // Should I retake the imperative programming class?
        size_t nbytes = 250;
        int readchars = getline(&lpointer, &nbytes, stdin);

        char filename[PATH_MAX];
        sprintf(filename,"%s/.authmount_credentials", getenv("HOME"));

        FILE *f = fopen(filename, "w");
        fprintf(f,"%s:%s\n", getenv("USER") , line);
        fclose(f);

        conf->credentials_path = "authmount_credentials";

        printf("Credentials saved in ~/.authmount_credentials\n");
    }
}

int main(int argc, char *argv[]) {
    gtk_init(NULL,NULL);

    nfs_validate_non_root_usage();

    struct nfs_state *nfs_data;
    nfs_data = malloc(sizeof(struct nfs_state));
    if (nfs_data == NULL) {
        fprintf(stderr, "Unable to allocate memory.\n");
        return 1;
    }

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    memset(&conf, 0, sizeof(conf));
    fuse_opt_parse(&args, &conf, nfs_opts, nfs_opt_proc);

    nfs_validate_options(&conf);

    printf("Mounting in %s rooted to %s. Credentials accessible and editable in %s.\n", args.argv[args.argc - 1], conf.root_dir, conf.credentials_path);

    nfs_data->rootdir = realpath(conf.root_dir,NULL);

    printf("Calling main fuse library.\n");
    return(fuse_main(args.argc, args.argv, &nfs_oper, nfs_data));
}
