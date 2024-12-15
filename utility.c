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
    const long UPPER_BOUND = 3037000000;

    // Seed the random number generator
    srand(time(NULL));

    // Find the first prime
    while (1) {
        long num = generateRandomNumber(LOWER_BOUND, UPPER_BOUND);
        if (isPrime(num)) {
            *primeA = num;
            break;
        }
    }

    // Find the second prime
    while (1) {
        long num = generateRandomNumber(LOWER_BOUND, UPPER_BOUND);
        if (isPrime(num)) {
            *primeB = num;
            break;
        }
    }
}

// Helper function to calculate modular exponentiation
long mod_pow(long base, long exp, long modulus) {
    if (modulus == 1) return 0;
    
    long result = 1;
    base = base % modulus;
    
    while (exp > 0) {
        if (exp % 2 == 1) { // if exponent is odd, multiply one time and take mod. it's like a^3 = a^2 * a 
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus; // base ^ 2 % mod
        exp = exp >> 1; // exp div by 2
    }
    return result;
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
void encrypt(char* input, char* encrypted, long k, long m) {
    // if there's no correct pointer, stop
    if (!input || !encrypted) return;
    
    // required variables
    int input_len = strlen(input);
    int m_digits = snprintf(NULL, 0, "%ld", m);  // Get number of digits in m
    char* temp = malloc((m_digits + 1) * sizeof(char));
    encrypted[0] = '\0';  // Initialize output string
    
    // encrypt each character. 
    for (int i = 0; i < input_len; i++) {
        // convert each character to a number and apply RSA: c = m^k mod n
        long message = (long)input[i];
        long cipher = mod_pow(message, k, m);
        
        // convert to string with proper padding based on modulus digits. So like if m_digit = 4, 24 --> 0024
        snprintf(temp, m_digits + 1, "%0*ld", m_digits, cipher);
        strcat(encrypted, temp);
    }
    
    free(temp);
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
void decrypt(char* input, char* decrypted, long p, long q, long k) {
    // If there aren't correct pointers, stop
    if (!input || !decrypted) return;
    
    // Get the required variables
    long m = p * q;
    long phi = (p - 1) * (q - 1);
    long d = mod_inverse(k, phi);  // Calculate private key using provided k.
    int m_digits = snprintf(NULL, 0, "%ld", m);  // Get number of digits in m
    int input_len = strlen(input);

    // temp string to store the decrypted message.
    char* temp = malloc((m_digits + 1) * sizeof(char));
    int pos = 0;
    decrypted[0] = '\0';
    
    // Process input string in chunks of m_digits
    for (int i = 0; i < input_len; i += m_digits) {
        // Extract the next chunk of encrypted text
        strncpy(temp, input + i, m_digits);
        temp[m_digits] = '\0';
        
        // Convert to number and decrypt using private key: m = c^d mod n
        long cipher = atol(temp);
        long message = mod_pow(cipher, d, m);
        
        // Convert back to character
        decrypted[pos++] = (char)message;
        decrypted[pos] = '\0';
    }
    
    free(temp);
}