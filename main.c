#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Function to compute the greatest common divisor (GCD) using Euclid's algorithm
unsigned int gcd(unsigned int a, unsigned int b) {
    while (b != 0) {
        unsigned int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Function to compute modular exponentiation (base^exp % mod)
unsigned int mod_exp(unsigned int base, unsigned int exp, unsigned int mod) {
    unsigned int result = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp % 2 == 1) {  // If exp is odd
            result = (result * base) % mod;
        }
        exp = exp >> 1;  // exp = exp / 2
        base = (base * base) % mod;
    }
    return result;
}

// Function to compute the modular inverse of `a` mod `m` (using Extended Euclidean Algorithm)
unsigned int mod_inverse(unsigned int a, unsigned int m) {
    unsigned int m0 = m, y = 0, x = 1;
    if (m == 1) return 0;
    
    while (a > 1) {
        unsigned int q = a / m;
        unsigned int t = m;
        m = a % m;
        a = t;
        t = y;
        y = x - q * y;
        x = t;
    }
    
    if (x < 0) x += m0;
    
    return x;
}

// Function to generate the RSA key pair (public and private keys)
void generate_rsa_keys(unsigned int p, unsigned int q, unsigned int *n, unsigned int *e, unsigned int *d) {
    *n = p * q;
    unsigned int phi_n = (p - 1) * (q - 1);
    *e = 65537;
    while (gcd(*e, phi_n) != 1) {
        (*e)++;
    }
    *d = mod_inverse(*e, phi_n);
}

// Function to encrypt a string using RSA
void rsa_encrypt(const char *message, unsigned int e, unsigned int n, unsigned int *encrypted) {
    size_t len = strlen(message);
    for (size_t i = 0; i < len; i++) {
        unsigned int char_val = (unsigned int) message[i];
        encrypted[i] = mod_exp(char_val, e, n);
    }
}

// Function to decrypt an encrypted message using RSA
void rsa_decrypt(unsigned int *encrypted, size_t len, unsigned int d, unsigned int n) {
    printf("\nDecrypted message: ");
    for (size_t i = 0; i < len; i++) {
        unsigned int decrypted_char = mod_exp(encrypted[i], d, n);
        
        // Ensure that the decrypted character is in the ASCII range (0-255)
        char decrypted_char_ascii = (char)(decrypted_char % 256);  // Map to 0-255 range
        
        // Print the decrypted character
        printf("%c", decrypted_char_ascii);
    }
    printf("\n");
}

int main() {
    unsigned int p = 1327356683;  // First prime number
    unsigned int q = 1327356697;  // Second prime number
    unsigned int n, e, d;

    // Generate the RSA key pair
    generate_rsa_keys(p, q, &n, &e, &d);

    printf("Generated RSA public key (n, e): (%u, %u)\n", n, e);
    printf("Generated RSA private key (n, d): (%u, %u)\n", n, d);

    const char *message = "Hello, RSA Encryption!";
    unsigned int encrypted[strlen(message)];

    // Encrypt the message using RSA
    rsa_encrypt(message, e, n, encrypted);

    printf("\nEncrypted message (integer values): ");
    for (size_t i = 0; i < strlen(message); i++) {
        printf("%u ", encrypted[i]);
    }
    printf("\n");

    // Decrypt the message using RSA
    rsa_decrypt(encrypted, strlen(message), d, n);

    return 0;
}
