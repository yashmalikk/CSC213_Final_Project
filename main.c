/* File: main.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "prime.h"
#include "utils.h"

#define HIDDEN_FOLDER ".secure_folder"
#define HASH_LENGTH 10

int main() {
    char userid[9]; // 8 characters + null terminator

    printf("Enter an 8-character lowercase user ID: ");
    scanf("%8s", userid);

    // Step 1: Create hidden folder
    if (create_hidden_folder(HIDDEN_FOLDER) != 0) {
        fprintf(stderr, "Error: Could not create hidden folder.\n");
        return EXIT_FAILURE;
    }

    // Step 2: Generate hash of user ID
    unsigned char hash[HASH_LENGTH];
    generate_hash(userid, hash);

    printf("Hash of user ID generated successfully.\n");

    // Step 3: Derive primes from the hash
    unsigned long prime1 = derive_prime(hash, 0);
    unsigned long prime2 = derive_prime(hash, 1);

    printf("Prime 1: %lu\n", prime1);
    printf("Prime 2: %lu\n", prime2);

    // Step 4: Save primes to file in hidden folder
    if (save_primes_to_file(HIDDEN_FOLDER, prime1, prime2) != 0) {
        fprintf(stderr, "Error: Could not save primes to file.\n");
        return EXIT_FAILURE;
    }

    printf("Primes saved successfully in hidden folder.\n");

    // RSA preparation can follow here
    return EXIT_SUCCESS;
}
