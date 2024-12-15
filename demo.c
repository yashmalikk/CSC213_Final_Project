#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utility.h"

#define MAX_MESSAGE_LENGTH 1024
#define MAX_ENCRYPTED_LENGTH 8192  // Larger buffer for encrypted text

// Function declarations from utility.c that we'll need
extern void twoPrimes(long* primeA, long* primeB);
extern void encrypt(char* input, char* encrypted, long k, long m);
extern void decrypt(char* input, char* decrypted, long p, long q, long k);

int main() {
    // Initialize variables
    long p, q;          // Our two prime numbers
    long k = 65537;     // Common RSA public exponent (e)
    char message[MAX_MESSAGE_LENGTH];
    char encrypted[MAX_ENCRYPTED_LENGTH];
    char decrypted[MAX_MESSAGE_LENGTH];
    
    // Generate two random prime numbers
    printf("Generating prime numbers...\n");
    twoPrimes(&p, &q);
    printf("Generated primes: p = %ld, q = %ld\n", p, q);
    
    // Calculate modulus
    long m = p * q;
    printf("Modulus (m = p * q): %ld\n", m);
    
    // Get message from user
    printf("\nEnter a message to encrypt: ");
    if (fgets(message, MAX_MESSAGE_LENGTH, stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }
    message[strcspn(message, "\n")] = '\0';  // Remove trailing newline
    
    // Encrypt the message
    printf("\nEncrypting message...\n");
    encrypt(message, encrypted, k, m);
    printf("Encrypted message: %s\n", encrypted);
    
    // Decrypt the message
    printf("\nDecrypting message...\n");
    decrypt(encrypted, decrypted, p, q, k);
    printf("Decrypted message: %s\n", decrypted);
    
    // Verify the decryption
    if (strcmp(message, decrypted) == 0) {
        printf("\nSuccess! The decrypted message matches the original.\n");
    } else {
        printf("\nError: The decrypted message does not match the original.\n");
        printf("Original: %s\n", message);
        printf("Decrypted: %s\n", decrypted);
    }
    
    return 0;
}