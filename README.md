# Simple Archive Utility

This utility is a simple archiving tool that allows you to create an archive of multiple files, list the contents of an archive, and extract files from an archive. The utility is written in C and demonstrates basic file I/O operations, including reading from and writing to files in binary mode.

## Features

- Create archives with multiple files
- List contents of an archive
- Extract files from an archive

## Compilation

To compile the archive utility, you will need a C compiler such as `gcc`. Use the following command to compile:

```
gcc -o archive_utility main.c
```

This will produce an executable named `archive_utility`.

## Usage

### Creating an Archive

To create an archive, use the `--create` flag followed by the `--file` flag to specify the name of the archive, and then list the files you wish to include in the archive.

```
./archive_utility --create --file archive.arc file1.txt file2.txt file3.txt
```

### Listing the Contents of an Archive

To list the contents of an archive, use the `--list` flag followed by the `--file` flag and the name of the archive.

```
./archive_utility --list --file archive.arc
```

### Extracting Files from an Archive

To extract files from an archive, use the `--extract` flag followed by the `--file` flag and the name of the archive.

```
./archive_utility --extract --file archive.arc
```

## Archive File Structure

The archive file format is simple:

- The first part contains the number of files archived.
- The second part stores the maximum length of file names.
- For each file, the archive stores the file name (padded to the maximum length) followed by the file size.
- After the metadata section, the archive contains the actual data of each file, stored sequentially.

## Limitations

- The utility does not support directory archiving.
- There is no compression; files are stored as-is.
- Error handling is basic, and the utility may not handle all edge cases gracefully.
