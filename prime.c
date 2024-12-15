/* File: prime.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "prime.h"

#define HASH_CHUNK_SIZE 4 // Use 4 bytes of the hash to create a number

// Helper function to check if a number is prime
int is_prime(unsigned long num) {
    if (num < 2) return 0;
    for (unsigned long i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

// Derive a prime number from the hash
unsigned long derive_prime(const unsigned char *hash, int index) {
    unsigned long candidate = 0;

    // Extract 4 bytes from the hash starting at the given index
    for (int i = 0; i < HASH_CHUNK_SIZE; i++) {
        candidate = (candidate << 8) | hash[(index * HASH_CHUNK_SIZE + i) % HASH_LENGTH];
    }

    // Ensure candidate is odd (prime numbers > 2 are odd)
    if (candidate % 2 == 0) candidate++;

    // Find the next prime number
    while (!is_prime(candidate)) {
        candidate += 2;
    }

    return candidate;
}
