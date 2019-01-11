#ifndef _PARAMS_H_
#define _PARAMS_H_


#define FUSE_USE_VERSION 26

#define _XOPEN_SOURCE 500

#include <limits.h>
#include <stdio.h>

struct nfs_state {
    char *rootdir;
};
#define NFS_DATA ((struct nfs_state *) fuse_get_context()->private_data)

#endif
