## Purpose
This program returns the longest common byte strand that is shared by two files out of a multitude of files.

## Sample Outputs
The output for files 1 to 10 are listed in this directory in "output.txt".

## Requirements
An install of a C compiler such as gcc.

## Usage
Only instructions for Linux and Mac are listed. If you have gcc installed, simply run: 

```
gcc main.c -o <filename>
```

This will generate an executable named `<filename>`. To run this executable, run:
```
`./<filename>`
```

Then, it will prompt you for a number of files. If the number typed is n, then files "sample.1", "sample.2", ... "sample.n" are scanned.
```
How many files would you like to scan?
10
```