#pragma once
#include <cstdint>

#include "function.hpp"
#include "rng.hpp"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace mc {

// Последователна оценка: един поток, един натрупващ сумарен регистър.
inline double estimate_sequential(uint64_t seed, uint64_t n) {
    double sum = 0.0;
    for (uint64_t i = 0; i < n; ++i) {
        double x = sample_in(seed, i, kA, kB);
        sum += f(x);
    }
    return (kB - kA) * sum / static_cast<double>(n);
}

// Паралелна оценка с OpenMP: статично разпределение на индексите между
// нишките (schedule(static)) и обединяване чрез reduction(+:sum), изцяло
// управлявано от компилатора/runtime — за разлика от явното делене на
// диапазони и Future-базираното събиране в Java версията (MonteCarlo.java).
inline double estimate_parallel(uint64_t seed, uint64_t n, int threads) {
    double sum = 0.0;
    (void)threads;
#ifdef _OPENMP
    #pragma omp parallel for num_threads(threads) reduction(+ : sum) schedule(static)
#endif
    for (int64_t i = 0; i < static_cast<int64_t>(n); ++i) {
        double x = sample_in(seed, static_cast<uint64_t>(i), kA, kB);
        sum += f(x);
    }
    return (kB - kA) * sum / static_cast<double>(n);
}

} // namespace mc
