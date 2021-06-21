#include <stdio.h>
#include "blumblumshub.h"
// two blum primes, stolen from a Wolfram Mathematica tutorial paper
#define PRIMEP "1267650600228229401496703981519"
#define PRIMEQ "1267650600228229401496704318359"

// Global variables! What if they change unpredictably!? :P
mpz_t x0;
mpz_t n;

long rand_num() {
    mpz_t num;
    mpz_init(num);
    mpz_pow_ui(x0, x0, 2);
    mpz_mod(num, x0, n);
    mpz_set(x0, num);
    long value = mpz_get_si(num);
    mpz_clear(num);
    return value;
}

void init_seed(unsigned long seed) {
    rand_init();
    set_seed(seed);
}

// seed should be between 2 and n-1
void set_seed(unsigned long seed) {
    mpz_set_ui(x0, seed);
}

void rand_init() {
    mpz_t p, q;
    mpz_inits(n, x0, p, q, NULL);
    mpz_set_str(p, PRIMEP, 10);
    mpz_set_str(q, PRIMEQ, 10);
    mpz_mul(n, p, q);
    mpz_clears(p, q, NULL);
}

void rand_clear() {
    mpz_clears(n, x0, NULL);
}
