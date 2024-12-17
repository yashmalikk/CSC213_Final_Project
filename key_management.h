#pragma once
#include <stdbool.h>

typedef struct {
    long public_key;
    long modulus;
} PublicKeyPair;

typedef struct {
    long public_key;  // e
    long phi;         // φ(m)
    long modulus;     // m = p*q
} PrivateKeyPair;

// Initialize the key management system (creates .keys directory if needed)
bool init_key_management(void);

// Check if we have a stored key for a given port
bool has_stored_key(unsigned short port);

// Store a public key for a given port
bool store_key(unsigned short port, PublicKeyPair key);

// Load a public key for a given port
bool load_key(unsigned short port, PublicKeyPair* key);

// Generate new key pair
void generate_key_pair(PublicKeyPair* public_key, PrivateKeyPair* private_key);
