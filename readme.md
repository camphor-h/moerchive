# What's this?
Moerchive is a simple files archive tool. The moerchive file has a clear structure. Its filename extension is ".mar".

There has two code file. One for archive (mar), the other for unarchive (unmar).

# What's its main feature?
* Direct data structure, instead of using fixed size data chunks.

A moerchive file has the following file data structure.

1. File Signature
2. File Header (How many files it archived)
3. File Chunks (Detail of files)

* Cross-platform

Through test, this program can run in Windows/Unix-Like(Linux FreeBSD MacOS) system.

* No third-party lib

I use pure local C lib to write this program. No need to download third-party lib.

# What's its main fault?

* No file-tree system

This program can only support archive file, no directory.

* File size limit

It can only write/read files whose size is in 4GB.

# What's its usage?

## ARCHIVE

mar [FILE1] [FILE2] [FILE3]... [OUTPUT_FILE_PATH]

Or you can use:

mar -a [SOURCE_PATH] [OUTPUT_FILE_PATH]

It will archive all file in [SOURCE_PATH]

## UNARCHIVE

unmar [MAR_FILE]

Files will be unarchived in current path.

unmar [MAR_FILE] [OUTPUT_FILE_DIRECTORY]

Files will be unarchived in [OUTPUT_FILE_DIRECTORY].

unmar -i [MAR_FILE]

This will only output the infomation about files the [MAR_FILE] archived.

# Compile

It now support two compile. GCC and Clang. If you use msvc, or any other C compile. Please modify Makefile by yourself.

You can use "make" to compile both of two program.

or use "make mar"/"make unmar" to compile single program.




Anyway, that's all. If you have any idea, or any bug to report. Please contact me. 




PolarCamphor