import os, fcntl

_IOC_NONE     = 0
_IOC_WRITE    = 1
_IOC_READ     = 2

_IOC_NRBITS	  = 8
_IOC_TYPEBITS = 8


_IOC_SIZEBITS = 14

_IOC_NRSHIFT = 0
_IOC_TYPESHIFT = _IOC_NRSHIFT+_IOC_NRBITS
_IOC_SIZESHIFT = _IOC_TYPESHIFT+_IOC_TYPEBITS
_IOC_DIRSHIFT  = _IOC_SIZESHIFT+_IOC_SIZEBITS

def _IOC(dir, type, nr, size) -> int:
    return (((dir)  << _IOC_DIRSHIFT) | 
	        ((type) << _IOC_TYPESHIFT) | 
	        ((nr)   << _IOC_NRSHIFT) | 
	        ((size) << _IOC_SIZESHIFT))

def _IO(type, nr) -> int:
    return _IOC(_IOC_NONE, ord(type), nr, 0)

SFS_IOC_MAGIC = 'S'
SFS_IOC_SEEK  =_IO(SFS_IOC_MAGIC, 0)

with open("/home/deadpool/Desktop/Programming/MyWork/user"
          "-land-filesystem/fs/samples/tests/mnt/j", "r+") as f:
    contents = f.readlines()
    for content in contents:
        print(content)
    
    f.truncate(0)

    contents = f.readlines()
    for content in contents:
        print(content)