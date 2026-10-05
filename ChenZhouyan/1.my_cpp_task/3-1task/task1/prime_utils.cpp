#include "prime_utils.h"

using namespace std;

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int m = 2; m * m <= n; ++m) {
        if (n % m == 0) return false;
    }
    return true;
}

vector<int> primesUpTo(int n) {
    vector<int> result;
    for (int i = 2; i <= n; ++i) {
        if (isPrime(i)) result.push_back(i);
    }
    return result;
}

int countPrimes(int n) {
    return (int)primesUpTo(n).size();
}
