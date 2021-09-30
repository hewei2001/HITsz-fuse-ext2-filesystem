#ifndef DISKOPS
#define DISKOPS

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include "fstree.h"
#include "bitmap.h"

/*
Initial size of the data and metadata disk files when created
*/
#define INIT_SIZE 1048576

/*
Block size of the metadata disk file
*/
#define BLOCK_SIZE 4096

/*
Open marker - Defines the beginning of a node in the metadata disk file
*/
#define OPEN_MARKER "{\n"

/*
Close marker - Defines the end of a node in the metadata disk file
*/
#define CLOSE_MARKER "}\n"

/* 
Data file descriptor
*/
int data_fd;

/* 
Metadata file descriptor
*/
int meta_fd;

/* 
Size of the data disk bitmap is stored in this variable when loaded into memory
*/
uint64_t datamap_size;

/* 
Data disk bitmap is stored in this variable when loaded into memory
*/
uint8_t * datamap;

/* 
Size of the metadata disk bitmap is stored in this variable when loaded into memory
*/
uint64_t metamap_size;

/* 
Metadata disk bitmap is stored in this variable when loaded into memory
*/
uint8_t * metamap;

/*
FStree Typedef to maintain consistency across files
*/
typedef struct FStree FStree;  

/*
FSfile Typedef to maintain consistency across files
*/
typedef struct FSfile FSfile;

/* 
Reset data file descriptor - signifies that data file is closed 
*/
void resetdatafd();

/*
Reset metadata file descriptor - signifies that metadata file is closed
*/
void resetmetafd();

/*
Writes the given bitmap to the file corresponding to the file descriptor 'fd'
*/
void writebitmap(int fd, uint8_t * bitmap, uint64_t bitmap_size);

/*
Loads the bitmap corresponding to the file given by 'fd' into memory
*/
void loadbitmap(int fd, uint8_t ** bitmap, uint64_t * bitmap_size);

/*
Creates the metadata and data disk files if not present, else, loads the metadata into memory
*/
int createdisk();

/*
Finds the first available block in the bitmap given by 'bitmap'
*/
unsigned long int find_free_block(uint8_t * bitmap, uint64_t bitmap_size);

/*
Writes a tree node, 'node', to the diskfile given by file descriptor 'fd' using the 'bitmap' of that file
*/
void write_diskfile(int fd, uint8_t * bitmap, uint64_t bitmap_size, FStree * node);

/*
Preprocesses information pertaining to the tree node 'temp' and passes it to write_diskfile along with other parameters known to this file
*/
void serialize_metadata(FStree * temp);

/*
Wrapper function for 'serialize_metadata'. Acts as an interface to the disk as the only parameter is the tree node and nothing else.
*/
void serialize_metadata_wrapper(FStree * node);

/*
Fetches the block number of the parent node of a given node, 'node'
*/
unsigned long int get_parent_block(int fd, FStree * node, int child_blocknumber);

/*
Gets the next metadata block number for chained data
*/
unsigned long int get_chained_meta_block(int fd, unsigned long int parent_blocknumber, unsigned long int child_blocknumber);

/*
Updates a given tree node, given by 'node' in the disk and mode signifies that the datablocks should be updated as well
*/
int update_node(int fd, uint8_t * bitmap, uint64_t bitmap_size, FStree * node, int mode);

/*
Wrapper function for update_node. Acts as an interface as the only parameter to this is the tree node 'node' and mode signifies that the datablocks should be updated as well
*/
int update_node_wrapper(FStree * node, int mode);

/*
Load the metadata from disk into memory starting at blocknumber 'blknumber' (usually the root block)
*/
void deserialize_metadata(unsigned long int blknumber);

/*
Wrapper function for deserialize_metadata. Acts as an interface as no parameters are required for the same.
*/
void deserialize_metadata_wrapper();

/*
Deletes a block from the disk file given by the inode number 'blocknumber'
*/
void delete_metadata_block(char * type,unsigned long int blocknumber);

/*
Checks if a given block is valid, i.e if the data in the block is deleted / exists in the FS
*/
int check_validity_block(unsigned long int blocknumber);

/*
Writes the data of a node given by 'node' to the data disk file
*/
void write_data(int fd, uint8_t * bitmap, uint64_t bitmap_size,unsigned long int inode,char * data,FStree * node);

/*
Serialises file data i.e writes the data corresponding to a given file to the data disk file
*/
void serialize_filedata(unsigned long int inode,char * data,FStree * node);

/*
Wrapper function for serialize_filedata. Acts as an interface for the same
*/
void serialize_filedata_wrapper(unsigned long int inode,char * data,FStree * node);

/*
Fetch the block number of a given data block corresponding to a particular file
*/
unsigned long int find_data_block(unsigned long int blocknumber);

/*
Loads the file data into memory upon read / write requests
*/
char * deserialize_file_data(unsigned long int inode);

#endif
