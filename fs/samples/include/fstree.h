#ifndef FSTREE
#define FSTREE

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "fsdisk.h"

/*
Structure of a data node
*/
struct FSfile{
	char * path;        // Path upto file
	char * name;        // Name of the file
	char * data;        // Data 
	long int offset;    // Offset within the file
	off_t size;         // Size of the file
};

/*
Structure of a metadata tree node
*/
struct FStree{
    char * path;                    // Path upto node
    char * name;                    // Name of the file / directory
    char * type;                    // Type : "directory" or "file"
    mode_t permissions;		        // Permissions 
    uid_t user_id;		            // userid
    gid_t group_id;		            // groupid
    int num_children;               // Number of children nodes
    int num_files;		            // Number of files
    time_t a_time;                  // Access time
    time_t m_time;                  // Modified time
    time_t c_time;                  // Status change time
    time_t b_time;                  // Creation time
    off_t size;                     // Size of the node
    unsigned long int inode_number; // Inode number of the node in disk    
    struct FStree * parent;         // Pointer to parent node
    struct FStree ** children;      // Pointers to children nodes
    struct FSfile ** fchildren;     // Pointers to files in the directory
};

/*
Typedef to maintain consistency across files
*/
typedef struct FStree FStree;  

/*
Typedef to maintain consistency across files
*/
typedef struct FSfile FSfile;

/*
Global time variable used to update the times of access, modification and status change times
*/
time_t t;

/*
Pointer to the root of the metadata tree
*/
extern FStree * root;       

/*
Function to extract next directory in a given path. E.g "a/b/c" returns "a" and changes the input string to "b/c"
*/
char * extract_path(char ** copy_path);

/*
General purpose function to reverse the content of a string. Mode signifies use for dropping extra '/' in directory.

E.g "/a/d/" should produce current dir as 'd' and not '/'
*/
char * reverse(char * str, int mode);

/*
Function to extract current directory in a given path. E.g "/a/b/c" returns "c"
*/
char * extract_dir(char ** copy_path);

/*
Function to search for a node in the FS tree, given the path
*/
FStree * search_node(char * path);

/*
Function to initialise an FS tree node
*/
FStree * init_node(const char * path, char * name, FStree * parent,int type);

/*
Function to insert a node into the FS tree
*/
void insert_node(const char * path);

/*
Function to intialise a file node
*/
FSfile * init_file(const char * path,char * name);

/*
Function to insert file into FStree
*/
void insert_file(const char * path);

/*
Function to delete file from FStree
*/
void delete_file(const char *path);

/*
Function to delete a node in the FS tree.
*/
int delete_node(const char * path);

/*
Function to search for a file in FStree. Returns a pointer to the file node.
*/
FSfile * find_file(const char * path);

/*
Moves a file or directory from src to dst
*/
void move_node(const char * from,const char * to);

/*
Function used by move to update the paths of the moved files / directories
*/
void path_update(FStree * dir_node,char * topath);

/*
Loads a tree node from disk file into the tree structure
*/
void load_node(char * path, char * type, gid_t groupid, uid_t userid, time_t lc_time, time_t lm_time, time_t la_time, time_t lb_time, unsigned long int inode, off_t size, mode_t lpermissions);

/*
Loads a file from disk file for read / write
*/
void load_file(const char *path, char * data);

#endif
