#include "key_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "utility.h"  // for twoPrimes function

/**
 * Returns true if there is directory, or if there isn't directory, create one.
 */
bool init_key_management() {
    struct stat st = {0};
    if (stat(".keys", &st) == -1) {
        // Directory doesn't exist, create it
        return mkdir(".keys", 0700) == 0;
    }
    return true;
}

/**
 * Gets the path for the key file
 * \param port        The port number of the peer
 * \param path        Buffer where the constructed path will be stored
 * \param path_size   Size of the path buffer
 */
static void get_key_path(unsigned short port, char* path, size_t path_size) {
    snprintf(path, path_size, ".keys/peer_%u.key", port);
}

/**
 * Returns true if the path has a stored key
 * \param port The port number
 */
bool has_stored_key(unsigned short port) {
    char path[256];
    get_key_path(port, path, sizeof(path));
    
    FILE* file = fopen(path, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

/**
 * Store the public key for certain port
 * \param port The port number
 * \param key Public key
 */
bool store_key(unsigned short port, PublicKeyPair key) {
    char path[256];
    get_key_path(port, path, sizeof(path));
    
    FILE* file = fopen(path, "w");
    if (!file) return false;
    
    fprintf(file, "%ld\n%ld", key.public_key, key.modulus);
    fclose(file);
    return true;
}

/**
 * Load the public key for certain port
 * \param port The port number
 * \param key Public key
 */
bool load_key(unsigned short port, PublicKeyPair* key) {
    char path[256];
    get_key_path(port, path, sizeof(path));
    
    FILE* file = fopen(path, "r");
    if (!file) return false;
    
    if (fscanf(file, "%ld\n%ld", &key->public_key, &key->modulus) != 2) {
        fclose(file);
        return false;
    }
    
    fclose(file);
    return true;
}

/**
 * \param public_key pointer that will store the public key to be generated
 * \param private_key pointer that will store the private key to be generated
 */
void generate_key_pair(PublicKeyPair* public_key, PrivateKeyPair* private_key) {
    // Generate two random primes
    long p, q;
    twoPrimes(&p, &q);
    
    // Calculate modulus
    long m = p * q;
    
    // Calculate Euler's totient
    long phi = (p - 1) * (q - 1);
    
    // Choose public exponent (commonly 65537)
    long e = 65537;  // A common choice that's efficient and secure
    
    // Set up public key
    public_key->public_key = e;
    public_key->modulus = m;
    
    // Set up private key - store phi(m) instead of d (or mu)
    private_key->public_key = e;
    private_key->phi = phi;
    private_key->modulus = m;
}