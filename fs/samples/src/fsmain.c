#define FUSE_USE_VERSION 25

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include "../include/fstree.h"
#include "../include/fsoperations.h"
#include "../include/bitmap.h"
#include "../include/fsdisk.h"

extern FStree * root;

static struct fuse_operations operations = {
        .getattr	= do_getattr,
        .readdir	= do_readdir,
        .mkdir      = do_mkdir,
        .rmdir      = do_rmdir,
        .mknod		= do_mknod,
        .open       = do_open,
        .read		= do_read,
        .unlink     = do_unlink,
        .chmod		= do_chmod,
        .write		= do_write,
	    .truncate   = do_truncate,
        .utime	    = do_utimens,
        .access	    = do_access,
        .rename     = do_rename,
};

int main( int argc, char *argv[] ){
    int ret = createdisk();
    if(ret){
        serialize_metadata_wrapper(root);
    }
    return fuse_main(argc, argv, &operations);
}
