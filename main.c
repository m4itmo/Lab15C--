#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void archive_files(int count, char *files[], const char *archive_filename) {
    FILE *archive_file = fopen(archive_filename, "wb");

    if (archive_file == NULL) {
        perror("Error opening archive file");
        exit(1);
    }


    size_t max_filename_len = 0;
    for (int i = 0; i < count; ++i) {
        size_t len = strlen(files[i]) + 1;
        if (len > max_filename_len) {
            max_filename_len = len;
        }
    }

    fwrite(&count, sizeof(count), 1, archive_file);  // NUMBER OF FILES
    fwrite(&max_filename_len, sizeof(max_filename_len), 1, archive_file);  // MAX FILE NAME

    for (int i = 0; i < count; ++i) {
        char *filename = calloc(max_filename_len, sizeof(char));
        strcpy(filename, files[i]);

        FILE *input_file = fopen(files[i], "rb");
        if (input_file == NULL) {
            perror("Error opening input file");
            continue;
        }

        fseek(input_file, 0, SEEK_END);
        long filesize = ftell(input_file);
        fseek(input_file, 0, SEEK_SET);

        fwrite(filename, sizeof(char), max_filename_len, archive_file);  // FILE NAME
        fwrite(&filesize, sizeof(filesize), 1, archive_file);  // FILE SIZE
//        printf("%d\n", filesize);

        free(filename);
        fclose(input_file);
    }

    for (int i = 0; i < count; ++i) {
        FILE *input_file = fopen(files[i], "rb");
        if (input_file == NULL) {
            perror("Error opening input file");
            continue;
        }

        char buffer[1024];
        while (!feof(input_file)) {
            size_t read = fread(buffer, sizeof(char), sizeof(buffer), input_file);
            fwrite(buffer, sizeof(char), read, archive_file);
        }

        fclose(input_file);
    }

    fclose(archive_file);
}


void unarchive_files(const char *archive_filename) {
    FILE *archive_file = fopen(archive_filename, "rb");
    if (archive_file == NULL) {
        perror("Error opening archive file");
        exit(1);
    }

    int count;
    fread(&count, sizeof(count), 1, archive_file);

    size_t max_filename_len;
    fread(&max_filename_len, sizeof(max_filename_len), 1, archive_file);

    char **filenames = malloc(count * sizeof(char*));
    long *filesizes = malloc(count * sizeof(long));

    for (int i = 0; i < count; ++i) {
        filenames[i] = malloc(max_filename_len * sizeof(char));
        fread(filenames[i], sizeof(char), max_filename_len, archive_file);
        fread(&filesizes[i], sizeof(filesizes[i]), 1, archive_file);
    }

    for (int i = 0; i < count; ++i) {
        FILE *output_file = fopen(filenames[i], "wb");
        if (output_file == NULL) {
            perror("Error opening output file");
            continue;
        }

        char buffer[1024];
        long remaining = filesizes[i];
        while (remaining > 0) {
            size_t to_read = sizeof(buffer) < remaining ? sizeof(buffer) : remaining;
            size_t read = fread(buffer, sizeof(char), to_read, archive_file);
            fwrite(buffer, sizeof(char), read, output_file);
            remaining -= read;
        }

        fclose(output_file);
    }

    for (int i = 0; i < count; ++i) {
        free(filenames[i]);
    }
    free(filenames);
    free(filesizes);

    fclose(archive_file);
}


void list_archive_contents(const char *archive_filename) {
    FILE *archive_file = fopen(archive_filename, "rb");

    if (archive_file == NULL) {
        perror("Error opening archive file");
        exit(1);
    }

    int count;
    fread(&count, sizeof(count), 1, archive_file);

    size_t max_filename_len;
    fread(&max_filename_len, sizeof(max_filename_len), 1, archive_file);

    printf("List of files in the archive '%s':\n", archive_filename);

    for (int i = 0; i < count; ++i) {
        char *filename = calloc(max_filename_len, sizeof(char));
        long filesize;

        fread(filename, sizeof(char), max_filename_len, archive_file); // FILE NAME
        fread(&filesize, sizeof(filesize), 1, archive_file);  // FILE SIZE

        printf("%s\n", filename); // Changed from &filename to filename to print the string

        free(filename);
    }

    fclose(archive_file);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s --file <archive.arc> [--create | --extract | --list] [files...]\n", argv[0]);
        return 1;
    }

    char *archive_filename = NULL;
    int create = 0, extract = 0, list = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--file") == 0) {
            if (i + 1 < argc) {
                archive_filename = argv[++i];
            } else {
                fprintf(stderr, "Error: Missing archive file name after --file\n");
                return 1;
            }
        } else if (strcmp(argv[i], "--create") == 0) {
            create = 1;
        } else if (strcmp(argv[i], "--extract") == 0) {
            extract = 1;
        } else if (strcmp(argv[i], "--list") == 0) {
            list = 1;
        }
    }

    if (!archive_filename) {
        fprintf(stderr, "Error: Archive file not specified\n");
        return 1;
    }

    if (create + extract + list != 1) {
        fprintf(stderr, "Error: Specify one of the commands --create, --extract, or --list\n");
        return 1;
    }

    if (create) {
        printf("--create\n");
        archive_files(argc - 4, &argv[4], archive_filename);
    } else if (extract) {
        printf("--extract\n");
        unarchive_files(archive_filename);
    } else if (list) {
        list_archive_contents(archive_filename);
    }


    return 0;
}
