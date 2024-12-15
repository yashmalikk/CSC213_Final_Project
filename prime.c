#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

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

// Function to return two prime numbers based on the UID
void get_primes_based_on_uid(uid_t uid, int *prime1, int *prime2) {
    // Use the UID to generate two different primes deterministically
    int first_prime = next_prime(uid + 10);  // Adding some offset to get a prime greater than UID
    int second_prime = next_prime(first_prime + 10);  // Getting a second prime after the first

    // Store the result in the provided pointers
    *prime1 = first_prime;
    *prime2 = second_prime;
}