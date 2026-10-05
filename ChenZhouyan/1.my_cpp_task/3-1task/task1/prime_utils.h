#ifndef PRIME_UTILS_H
#define PRIME_UTILS_H

#include <vector>

// 判断 n 是否为质数（n <= 1 返回 false）
bool isPrime(int n);

// 返回 [2, n] 区间内所有质数
std::vector<int> primesUpTo(int n);

// 统计 [2, n] 区间内质数的个数
int countPrimes(int n);

#endif
