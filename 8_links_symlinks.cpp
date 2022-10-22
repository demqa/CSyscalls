
/*
links    (hard)
symlinks (soft)

DT_DIR
DT_REG
DT_SYM

stat struct contains info about links.

DIR .obj ----> i-node stat -> data
    .sss ----> i-node stat -> path

read gets data
readlink gets data of link: path and other info.

open ("sss") -> operating system get path and then do real open.
stat ("sss") -> will give info about real file.

lstat - gets info about link.

symlink can be broken. // different errno
symlink can be not a path. can contain some information.

DIR entries contains name and inodes.
we can create entries with different names but it will point to one file.
It is a hard link. ST_NLINKS contains number of links to file.
If nlinks == 0, then file will be removed?

Disk 1
Flash
USB-drive

Inside device there is another file system.
Hard link is a local name. It differs in file systems.
Symbolic links can point to other file system.
|----|
|    |
|    |
|----|

ls - -l flag

EXTRA:
Search all the entries of filesystem to find all hard links.
*/
