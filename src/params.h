#ifndef _PARAMS_H_
#define _PARAMS_H_

#ifdef __APPLE__
    #define _XOPEN_SOURCE 500
#elif __linux__
    #define _XOPEN_SOURCE 700
#else
#   error "Compilador nao suportado"
#endif

#define FUSE_USE_VERSION 26


#include <limits.h>
#include <stdio.h>

struct nfs_state {
        char *rootdir;
};
#define NFS_DATA ((struct nfs_state *) fuse_get_context()->private_data)

#endif
