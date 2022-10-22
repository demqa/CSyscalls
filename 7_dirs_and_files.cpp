/*

directory
list that contains pointers to children

index node
i_node

node  == file system node
index == unique node index

directory node

dir
  .
  ..
  bin  | #312
  name | i_node


#2
ls | #375

// cringe because differs from FS to FS (FILE SYSTEM)
getdent - get directory entry

// another way to universalize directory object
d = opendir(path);
// d == list of
while (entry = readdir(d)) {
    ..
}

closedir(d);

stat(path);
fstat(FD);
lstat(path); // symlink

// meta data - data about data
// data - file contains something (text, code, trash)

struct stat *
{
  st_ino
  st_size
  st_blocks
  st_mtime // modified
  st_ctime // create
  st_atime // access
  st_mode
}

*/
