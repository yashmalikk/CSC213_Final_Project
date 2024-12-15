#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include "prime.c"

int main() {
    // Get the current UID
    uid_t uid = getuid();

    // Declare variables to store the prime numbers
    int prime1, prime2;

    // Get the two primes based on the UID
    get_primes_based_on_uid(uid, &prime1, &prime2);

    // Print the result
    printf("For UID %u, the two primes are: %d and %d\n", uid, prime1, prime2);

    // Required n = p * q
    int n = prime1 * prime2;

    // Print the required n.
    printf("Required n: %d\n", n);
    return 0;
}
