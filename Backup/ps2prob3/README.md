**Problem 3 -- Filesystem scavenger hunt**

<details>
<summary><h3 style="display: inline">REQUIREMENT: </h3></summary>
<br>

You will write a program which will be given the name of an existing regular file (which is assumed to be readable by the invoking user), and a starting point pathname in the filesystem (which is assumed to be a directory name). It will recursively explore all of the nodes in the filesystem under that starting point, looking for files which are identical to the first file. Only inodes which are Regular Files or Symlinks should be considered as candidates.

We will define "identical" as follows: Two files are identical if they have the same number of bytes (size) and each and every byte of one file is the same as the byte at the same offset in the second file. Differences in metadata do not impact "identical". You do not need to consider "sparse allocation" in your determination of identical.

When you find a "hit" you will print out (to standard output) the following: - The pathname of the matching file. If the starting point was given as an absolute pathname, your output will also be an absolute pathname, but if the starting point was a relative pathname, you are not being asked to translate that
into an absolute pathname (doing so can be non-deterministic and requires use of a library function that will make a lot of system calls)

- If the target file has a link count >1 and you have encountered a hard link to the original target, output that fact. (In this case, you will also wind up outputting a "match" if your search area includes the original target. Don’t worry about this spurious output).
- Otherwise, if this matching file is a distinct inode from the target file, output the nlink value of the matching file. If nlink>1 it is possible that you will see this same matching inode multiple times during your search. You are not required to distinguish between multiple matches of distinct inodes and multiple links to a matching inode that is distinct from the original target inode.

If you encounter a symlink inode, there are two possible matching scenarios:
- The symlink resolves to the original target file inode. If so, report this.
- The symlink resolves to a distinct inode which happens to be identical in content to the target inode. If so, report this and also report the contents of the symlink. You may or may not see the inode to which the symlink resolves before or after seeing the symlink to it, depending on your search area.

Also, remember when doing your recursion, do NOT follow symlinks, otherwise you could potentially wind up in an endless loop.

ERRORS: Distinguish between critical and non-critical errors. E.g. if you are not able to open or stat the target file, this is a critical error, since there is no meaningful way to proceed. On the other hand, if you encounter an error opening a directory during your traversal, this should not be fatal. You simply will not be able to explore that part of the filesystem. Report all errors, fatal or warning, with proper descriptive error messages containing the 4 basic pieces of information as discussed in Unit #1. Error reports go to standard error, of course.

Efficiency: Do not perform unnecessary reads on a potential matching file. Think about ways to disqualify an inode as being a potential match without having to read the contents. For this assignment, you are permitted to use any system calls or stdio library functions to read the target and prospective matching files for comparison. Certain approaches might be more time efficient than others.

Binary: Your program must work with any file, including "binary" files (such as executables and media files) that contain non-printable characters. (Do you understand the distinction between strncmp and memcmp, for example?)

It is expected that you will use the standard library functions opendir, readdir and closedir to traverse the directories, and other system calls or library functions covered in Units #1 and #2 to perform the comparisons. If you are using functions that we haven’t covered, it is possible that you are making this too complicated. You are allowed to use ordinary standard library functions such as fprintf, strlen etc. You are not allowed to "shell out" to any external program such as diff. Any questions on what is permissible: please ask.

During the coding and testing of this program, you might find it helpful to insert additional debugging output, such as the pathname of each inode that you are considering. If you do this, please make sure to turn off the debugging output before making your final submission!

Below is some sample output. Your program is not required to produce identically formatted output! This is just an example to illustrate the results and the information that is expected.
```shell
$ ./hunt testtarget ..
DEBUG: Target is 13432 bytes long, dev 908 ino 14819843
Warning: Can’t open directory ../secret:Permission denied
../week2/hardlink HARD LINK TO TARGET
../week2/candidate1 DUPLICATE OF TARGET (nlink=2)
../week2/testtarget HARD LINK TO TARGET
../week2/candidate3 DUPLICATE OF TARGET (nlink=2)
DEBUG: ../week2/crazylink links to something not a file, skipping
../week1/goodlink SYMLINK RESOLVES TO TARGET
../week100/badlink1 SYMLINK (../week2/candidate1) RESOLVES TO DUPLICATE
../week100/badlink2 SYMLINK RESOLVES TO TARGET
../week100/badlink3 HARD LINK TO TARGET
```
</details>

# SAMPLE RUNS

### COMPILE: 
`gcc -o hunt p3.c`

### RUN:
`./hunt.out a/original a`

### DIRECTORY TREE:
```
a
├── b
│   └── lnk-ori
├── c
│   └── lnk-iden
├── d
│   └── snk-ori
├── e
│   └── snk-iden
├── identical
└── original
```
### STDOUT:
```
a/c/lnk-iden DUPLICATE OF TARGET (nlink=2)
a/identical DUPLICATE OF TARGET (nlink=2)
a/e/snk-iden SYMLINK (../identical) RESOLVES TO DUPLICATE
a/d/snk-ori SYMLINK (../original) RESOLVES TO TARGET
a/b/lnk-ori HARD LINK TO TARGET
a/original HARD LINK TO TARGET
```