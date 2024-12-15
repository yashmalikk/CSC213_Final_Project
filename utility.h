#pragma once

#include <stdbool.h>

// Prime number functions
bool is_prime(int num);
int next_prime(int start);
void twoPrimes(long* primeA, long* primeB);

// Mathematical helper functions
long mod_pow(long base, long exp, long modulus);
void extended_gcd(long a, long b, long *x, long *y);
long mod_inverse(long a, long m);
long generateRandomNumber(long lower, long upper);

// RSA encryption/decryption functions
/**
 * Encrypts a string using RSA.
 * 
 * @param input The input string to encrypt
 * @param encrypted The output buffer for the encrypted string
 * @param k The public exponent
 * @param m The modulus (product of two primes)
 */
void encrypt(char* input, char* encrypted, long k, long m);

/**
 * Decrypts an RSA-encrypted string.
 * 
 * @param input The encrypted string to decrypt
 * @param decrypted The output buffer for the decrypted string
 * @param p The first prime number used in encryption
 * @param q The second prime number used in encryption
 * @param k The public exponent used in encryption
 */
void decrypt(char* input, char* decrypted, long p, long q, long k);

