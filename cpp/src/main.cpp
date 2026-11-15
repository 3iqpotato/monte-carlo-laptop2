#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "monte_carlo.hpp"
#include "rng.hpp"

namespace {

struct Args {
    std::string mode = "seq"; // seq | par | verify
    uint64_t n = 1000000ULL;
    int threads = 1;
    uint64_t seed = 42ULL;
    int repeats = 1;
    int warmup = 1;
    std::string machine = "unknown";
};

Args parse_args(int argc, char** argv) {
    Args a;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        auto next = [&]() -> std::string {
            if (i + 1 >= argc) {
                std::fprintf(stderr, "Missing value for %s\n", arg.c_str());
                std::exit(1);
            }
            return argv[++i];
        };
        if (arg == "--mode") a.mode = next();
        else if (arg == "--n") a.n = std::strtoull(next().c_str(), nullptr, 10);
        else if (arg == "--threads") a.threads = std::atoi(next().c_str());
        else if (arg == "--seed") a.seed = std::strtoull(next().c_str(), nullptr, 10);
        else if (arg == "--repeats") a.repeats = std::atoi(next().c_str());
        else if (arg == "--warmup") a.warmup = std::atoi(next().c_str());
        else if (arg == "--machine") a.machine = next();
        else {
            std::fprintf(stderr, "Unknown argument: %s\n", arg.c_str());
            std::exit(1);
        }
    }
    return a;
}

// Отпечатва първите 10 (seed, index) хешове/равномерни проби - за сверка
// бит-по-бит с Java изхода (java Main --mode verify --seed <seed>).
void run_verify(uint64_t seed) {
    std::printf("index,hash_u64,uniform01_bits\n");
    for (uint64_t i = 0; i < 10; ++i) {
        uint64_t h = mc::hash_u64(seed, i);
        double u = mc::uniform01(seed, i);
        uint64_t bits;
        std::memcpy(&bits, &u, sizeof bits);
        std::printf("%llu,%llu,%016llx\n", static_cast<unsigned long long>(i),
                    static_cast<unsigned long long>(h),
                    static_cast<unsigned long long>(bits));
    }
}

} // namespace

int main(int argc, char** argv) {
    Args args = parse_args(argc, argv);

    if (args.mode == "verify") {
        run_verify(args.seed);
        return 0;
    }

    if (args.mode != "seq" && args.mode != "par") {
        std::fprintf(stderr, "Unknown mode: %s\n", args.mode.c_str());
        return 1;
    }
    auto run = [&]() {
        return args.mode == "seq"
                   ? mc::estimate_sequential(args.seed, args.n)
                   : mc::estimate_parallel(args.seed, args.n, args.threads);
    };

    // Неизмервани загряващи пускания (кеш, създаване на OpenMP нишковия пул).
    volatile double sink = 0.0;
    for (int w = 0; w < args.warmup; ++w) sink = sink + run();

    std::printf(
        "language,mode,n,threads,seed,repeat_index,time_seconds,estimate,"
        "abs_error,rel_error,samples_per_sec,machine\n");

    for (int r = 0; r < args.repeats; ++r) {
        auto t0 = std::chrono::steady_clock::now();
        double estimate = run();
        auto t1 = std::chrono::steady_clock::now();
        double seconds = std::chrono::duration<double>(t1 - t0).count();

        double abs_err = std::fabs(estimate - mc::kReference);
        double rel_err = abs_err / mc::kReference;
        double sps = seconds > 0 ? static_cast<double>(args.n) / seconds : 0.0;

        std::printf(
            "cpp,%s,%llu,%d,%llu,%d,%.9f,%.12f,%.12e,%.12e,%.6f,%s\n",
            args.mode.c_str(), static_cast<unsigned long long>(args.n),
            args.threads, static_cast<unsigned long long>(args.seed), r,
            seconds, estimate, abs_err, rel_err, sps, args.machine.c_str());
    }
    return 0;
}
