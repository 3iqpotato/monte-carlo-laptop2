#pragma once
#include <cstdint>

namespace mc {

// Детерминирана, безсъстоятелна (stateless) хеш-базирана PRNG.
//
// Използва смесващата функция на SplitMix64 (Vigna) като хеш на двойка
// (seed, index), вместо класическата последователна употреба със state,
// който се мутира на всяко повикване. Това има един ключов ефект за
// паралелната програма: пробата с индекс i може да се изчисли от коя да е
// нишка по кое да е време, без синхронизация и без да зависи от реда на
// изпълнение — затова C++ (OpenMP) и Java (java.util.concurrent) версиите
// получават бит-по-бит еднакви проби независимо от броя нишки.
//
// Еднаквите 64-битови константи и операции (включително unsigned shift)
// са възпроизведени идентично в java/src/Rng.java, за да съвпадат резултатите
// между двата езика — виж verify режима на двете main-програми.

inline uint64_t hash_u64(uint64_t seed, uint64_t index) {
    uint64_t z = seed + index * 0x9E3779B97F4A7C15ULL;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    z = z ^ (z >> 31);
    return z;
}

// Равномерно разпределено double в [0, 1) от горните 53 бита на хеша.
inline double uniform01(uint64_t seed, uint64_t index) {
    uint64_t h = hash_u64(seed, index);
    return static_cast<double>(h >> 11) * (1.0 / 9007199254740992.0); // 2^53
}

inline double sample_in(uint64_t seed, uint64_t index, double a, double b) {
    return a + (b - a) * uniform01(seed, index);
}

} // namespace mc
