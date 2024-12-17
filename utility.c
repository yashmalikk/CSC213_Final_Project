#include <string.h>
#include <stdlib.h>  
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>


// Function to check if a number is prime
bool is_prime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}

// Function to find the next prime number greater than or equal to 'start'
int next_prime(int start) {
    while (!is_prime(start)) {
        start++;
    }
    return start;
}

// Generate a random number between lower and upper (inclusive)
long generateRandomNumber(long lower, long upper) {
    // Calculate the range
    long range = upper - lower + 1;
    long rand_num = 0;

    // Calculate how many bits are needed to represent the upper limit
    int bits = 0;
    long temp = range;
    while (temp > 0) {
        bits++;
        temp >>= 1;
    }

    // Generate a random number within the range using multiple rand() calls if necessary
    do {
        rand_num = 0;
        for (int i = 0; i < bits / 15 + 1; i++) { // Assuming RAND_MAX >= 32767
            rand_num = (rand_num << 15) | (rand() & 0x7FFF);
        }
        rand_num = rand_num % range;
    } while (rand_num >= range);

    return lower + rand_num;
}

// Find two random primes within the specified range
void twoPrimes(long* primeA, long* primeB) {
    if (primeA == NULL || primeB == NULL) {
        fprintf(stderr, "Null pointer passed to twoPrimes.\n");
        return;
    }

    // these numbers are bcz 16*16 = 256 is the minimum for representing a single character.
    // 3037000000 is the limit for 'long' type when squared.
    const long LOWER_BOUND = 16;
    const long UPPER_BOUND = 10000; // previous: 3037000000

    // Seed the random number generator
    srand(time(NULL));

    // Find the first prime
    while (1) {
        long num = generateRandomNumber(LOWER_BOUND, UPPER_BOUND);
        if (is_prime(num)) {
            *primeA = num;
            break;
        }
    }

    // Find the second prime
    while (1) {
        long num = generateRandomNumber(LOWER_BOUND, UPPER_BOUND);
        if (is_prime(num)) {
            *primeB = num;
            break;
        }
    }
}

// Helper function to calculate modular exponentiation
long mod_pow(long base, long exp, long mod) {
    if (mod <= 0) {
        // Invalid modulus, handle as needed
        return -1;
    }

    long result = 1 % mod;    // In case mod == 1
    long b = base % mod;
    if (b < 0) b += mod;      // Ensure b is positive

    while (exp > 0) {
        if (exp & 1) {
            result = (result * b) % mod;
            if (result < 0) result += mod; // keep result positive
        }
        b = (b * b) % mod;
        if (b < 0) b += mod;  // keep b positive
        exp >>= 1;
    }
    return result; // result should now be between 0 and mod-1
}

// Helper function to calculate Extended Euclidean Algorithm
void extended_gcd(long a, long b, long *x, long *y) {
    if (a == 0) {
        *x = 0;
        *y = 1;
        return;
    }
    
    long x1, y1;
    extended_gcd(b % a, a, &x1, &y1);
    
    *x = y1 - (b/a) * x1;
    *y = x1;
}

// Helper function to calculate modular multiplicative inverse
long mod_inverse(long a, long m) {
    long x, y;
    extended_gcd(a, m, &x, &y);
    return (x % m + m) % m;
}


/**
 * Encrypts a string using RSA.
 * 
 * \param input input string. ex: "hey"
 * \param encrypted output string. ex: "3098093209458430"
 * \param k the exponent
 * \param m the moduli
 */
void rsa_encrypt(char* input, char* encrypted, long k, long m) {
    // if there's no correct pointer, stop
    if (!input || !encrypted) return;
    
    // required variables
    int input_len = strlen(input);
    int m_digits = snprintf(NULL, 0, "%ld", m);  // Get number of digits in m
    encrypted[0] = '\0';  // Initialize output string
    
    // encrypt each character
    for (int i = 0; i < input_len; i++) {
        // convert each character to a number and apply RSA: c = m^k mod n
        long message = (long)input[i];
        long cipher = mod_pow(message, k, m);
        if (cipher < 0) cipher += m; // ensure cipher is positive
        
        // Convert cipher to string with proper padding
        // The format string %0*ld will pad with leading zeros
        // m_digits specifies how many digits we want total
        char temp[64];
        snprintf(temp, sizeof(temp), "%0*ld", m_digits, cipher);
        
        // Append this block to our encrypted string
        strcat(encrypted, temp);
    }
}

/**
 * \param input encrypted string. ex "302923947953720598274058"
 * \param decrypted decrypted output string "hello"
 * \param p one of the two primes
 * \param q one of the two primes
 * \param k the exponent used in encryption. Need this to decrypt. 
 * 
 * each letter is represented by characters in l digits. l should be the digit of p*q.
 * For example, if the encryption was done with m = p*q = 17 * 101 = 1717, every letter was encrypted to be a number that is at max 4 digits. 
 * So every 4 digits in the encrypted string corresponds to a letter and should be decrypted. 
 * For example, if the given string is "320300420481", 3203 needs to be decrypted, 0042 needs to be decrypted, 0481 needs to be decrypted.
 * (assuming m is 4 digit still).
 * 
 */
void rsa_decrypt(char* input, char* decrypted, long p, long q, long k) {
    if (!input || !decrypted) return;
    
    long m = p * q;
    long phi = (p - 1) * (q - 1);
    long d = mod_inverse(k, phi);  // compute private exponent
    int m_digits = snprintf(NULL, 0, "%ld", m);
    int input_len = (int)strlen(input);
    
    int pos = 0;
    decrypted[0] = '\0';

    // Process input in chunks of m_digits
    for (int i = 0; i < input_len; i += m_digits) {
        char block[64] = {0};
        strncpy(block, input + i, m_digits);
        block[m_digits] = '\0';

        // Convert the block (zero-padded number) into a long
        long cipher = 0;
        for (int j = 0; block[j] != '\0'; j++) {
            cipher = cipher * 10 + (block[j] - '0');
        }
        
        // Decrypt the block: m = c^d mod n
        long message = mod_pow(cipher, d, m);

        // Convert back to character and append
        decrypted[pos++] = (char)message;
        decrypted[pos] = '\0';
    }
}