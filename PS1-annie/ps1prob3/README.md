# INSTRUCTION

`meow - concatenate and copy files`

### USAGE:

`meow [-o outfile] infile1 [...infile2....]`

`meow [-o outfile]`

### DESCRIPTION:

This program opens each of the named input files in order, and concatenates the entire contents of each file, in order, to the output. If an outfile is specified, meow opens that file (once) for writing, creating it if it did not already exist, and overwriting the contents if it did. If no outfile is specified, the output is written to standard output, which is assumed to already be open.

During the concatenation, meow will use a read/write buffer size of 4096 bytes.

Any of the infiles can be the special name - (a single hyphen). meow will then concatenate standard input to the output, reading until end-of-file, but will not attempt to re-open or to close standard input. The hyphen can be specified multiple times in the argument list, each of which will cause standard input to be read again at that point.

If no infiles are specified, meow reads from standard input until eof.

At the end of concatenating any file (including standard input), meow will print a message to standard error (not standard output) giving the name of the file, the number of bytes transferred, and the number of lines.

In the case of standard input, the name will appear as `<standard input>`

### EXIT STATUS:

program returns 0 if no errors (opening, reading, writing or closing) were encountered.

Otherwise, it terminates immediately upon the error, giving a proper error report, and returns -1.

### EXAMPLES:

`meow file1 - file 2`

(read from file1 until EOF, then standard input until EOF, then file 2, output to standard output)

`meow -o output - - file3`

(read from standard input until EOF, then read again from standard input until EOF, then read file3 until EOF, all output to file "output")

# SAMPLE RUNS

### COMPILE: 
`gcc -o meow p3.c`

<details>
<summary><h3 style="display: inline">SAMPLE 1:</h3></summary>
<br>

`./meow.out`
#### STDIN:
```
I am meowing
without any
argument
^D

```
#### STDOUT:
```
I am meowing
without any
argument

```
#### STDERR:
```
File: <standard input>, Bytes: 34, Lines: 4
```
</details>

<details>
<summary><h3 style="display: inline">SAMPLE 2:</h3></summary>
<br>

`./meow.out -o cattree.txt`
#### STDIN:
```
I am meowing
to the cat tree
^D

```
#### cattree.txt:
```
I am meowing
to the cat tree

```
#### STDERR:
```
File: <standard input>, Bytes: 29, Lines: 3
```
</details>

<details>
<summary><h3 style="display: inline">SAMPLE 3:</h3></summary>
<br>

`./meow.out -o rat.txt fancyfeast.txt - cattree.txt - cat.txt`
#### STDIN:
```

I am meowing oncee

^D

I am meowing twicee

^D

```
#### fancyfeast.txt:
```
Fancy Feast is a 
brand of gourmet cat food 
from Nestlé Purina PetCare. 
Introduced by the 
Carnation Company in 1982, 
it was originally offered in 
seven flavors of wet food.

```
#### cat.txt:
```
The cat 
(Felis catus) 
is a domestic species of 
small carnivorous mammal.

```
#### rat.txt:
```
Fancy Feast is a 
brand of gourmet cat food 
from Nestlé Purina PetCare. 
Introduced by the 
Carnation Company in 1982, 
it was originally offered in 
seven flavors of wet food.

I am meowing oncee

I am meowing
to the cat tree

I am meowing twicee

The cat 
(Felis catus) 
is a domestic species of 
small carnivorous mammal.

```
#### STDERR:
```

File: fancyfeast.txt, Bytes: 179, Lines: 8

File: <standard input>, Bytes: 21, Lines: 4

File: cattree.txt, Bytes: 29, Lines: 3

File: <standard input>, Bytes: 22, Lines: 4

File: cat.txt, Bytes: 76, Lines: 5
   
```
</details>

<details>
<summary><h3 style="display: inline">SAMPLE 4:</h3></summary>
<br>

`./meow.out -o no meowing today`
#### STDERR:
```
open(argv[argn], O_RDONLY): No such file or directory
```
</details>

<details>
<summary><h3 style="display: inline">SAMPLE 5:</h3></summary>
<br>

`./meow.out -o cannedfood.txt`
#### cannedfood.txt:
```
Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-ar--l          9/5/2023  11:42 PM              0 cannedfood.txt
```
#### STDERR:
```
open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666): Permission denied
```
</details>

<details>
<summary><h3 style="display: inline">SAMPLE 6:</h3></summary>
<br>

`./meow.out -o`
#### STDERR:
```
open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666): Invalid argument
```
</details>
