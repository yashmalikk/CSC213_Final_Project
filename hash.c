/* File: hash.c */
#include <openssl/sha.h>
#include <string.h>
#include "hash.h"

void generate_hash(const char *userid, unsigned char *output_hash) {
    // Initialize OpenSSL SHA-256 context
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    // Hash the user ID (8 characters)
    SHA256_Update(&sha256, userid, strlen(userid));

    // Finalize the hash computation
    SHA256_Final(output_hash, &sha256);
}
