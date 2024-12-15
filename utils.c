#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

// Create a hidden folder
int create_hidden_folder(const char *folder_name) {
    if (mkdir(folder_name, 0700) == -1) {
        perror("mkdir");
        return -1;
    }
    return 0;
}

// Save the primes to a file in the hidden folder
int save_primes_to_file(const char *folder_name, unsigned long prime1, unsigned long prime2) {
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/primes.txt", folder_name);

    FILE *file = fopen(file_path, "w");
    if (!file) {
        perror("fopen");
        return -1;
    }

    fprintf(file, "Prime 1: %lu\n", prime1);
    fprintf(file, "Prime 2: %lu\n", prime2);
    fclose(file);

    return 0;
}
