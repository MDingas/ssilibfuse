static void nfs_fullpath(char fpath[PATH_MAX], const char *path);
static int nfs_getattr(const char *path, struct stat *stbuf);
static int nfs_access(const char *path, int mask);
static int nfs_readlink(const char *path, char *buf, size_t size);
static int nfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int nfs_mknod(const char *path, mode_t mode, dev_t rdev);
static int nfs_mkdir(const char *path, mode_t mode);
static int nfs_unlink(const char *path);
static int nfs_rmdir(const char *path);
static int nfs_symlink(const char *path, const char *link);
static int nfs_rename(const char *path, const char *newpath);
static int nfs_link(const char *path, const char *newpath);
static int nfs_chmod(const char *path, mode_t mode);
static int nfs_chown(const char *path, uid_t uid, gid_t gid);
static int nfs_truncate(const char *path, off_t size);
#ifdef HAVE_UTIMENSAT
static int nfs_utimens(const char *path, const struct timespec ts[2]);
#endif
static int nfs_open(const char *path, struct fuse_file_info *fi);
static int nfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int nfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
static int nfs_statfs(const char *path, struct statvfs *stbuf);
static int nfs_release(const char *path, struct fuse_file_info *fi);
static int nfs_fsync(const char *path, int isdatasync, struct fuse_file_info *fi);
#ifdef HAVE_POSIX_FALLOCATE
static int nfs_fallocate(const char *path, int mode, off_t offset, off_t length, struct fuse_file_info *fi);
#endif
#ifdef HAVE_SETXATTR
static int nfs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags);
static int nfs_getxattr(const char *path, const char *name, char *value, size_t size);
static int nfs_listxattr(const char *path, char *list, size_t size);
static int nfs_removexattr(const char *path, const char *name);
#endif

static struct fuse_operations nfs_oper = {
    .getattr	= nfs_getattr,
    .access		= nfs_access,
    .readlink	= nfs_readlink,
    .readdir	= nfs_readdir,
    .mknod		= nfs_mknod,
    .mkdir		= nfs_mkdir,
    .symlink	= nfs_symlink,
    .unlink		= nfs_unlink,
    .rmdir		= nfs_rmdir,
    .rename		= nfs_rename,
    .link		= nfs_link,
    .chmod		= nfs_chmod,
    .chown		= nfs_chown,
    .truncate	= nfs_truncate,
#ifdef HAVE_UTIMENSAT
    .utimens	= nfs_utimens,
#endif
    .open		= nfs_open,
    .read		= nfs_read,
    .write		= nfs_write,
    .statfs		= nfs_statfs,
    .release	= nfs_release,
    .fsync		= nfs_fsync,
#ifdef HAVE_POSIX_FALLOCATE
    .fallocate	= nfs_fallocate,
#endif
#ifdef HAVE_SETXATTR
    .setxattr	= nfs_setxattr,
    .getxattr	= nfs_getxattr,
    .listxattr	= nfs_listxattr,
    .removexattr	= nfs_removexattr,
#endif
};

struct nfs_config {
    char* root_dir;
    char* mount_point;
    char* credentials_path;
};

enum {
    KEY_HELP,
    KEY_VERSION,
};

#define NFS_OPT(t, p, v) { t, offsetof(struct nfs_config, p), v }
