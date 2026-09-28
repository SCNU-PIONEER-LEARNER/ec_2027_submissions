#include "prime_utils.h"

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int m = 2; m * m <= n; ++m) {
        if (n % m == 0) return false;
    }
    return true;
}

std::vector<int> primesUpTo(int n) {
    std::vector<int> result;
    for (int i = 2; i <= n; ++i) {
        if (isPrime(i)) result.push_back(i);
    }
    return result;
}

int countPrimes(int n) {
    return static_cast<int>(primesUpTo(n).size());
}
