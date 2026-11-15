/**
 * Детерминирана, безсъстоятелна хеш-базирана PRNG — бит-по-бит идентична на
 * cpp/src/rng.hpp. Java няма unsigned типове, затова се ползва {@code >>>}
 * (unsigned shift); умножението на long дава същите 64 бита като unsigned
 * умножение в C++, така че хешовете съвпадат.
 */
public final class Rng {
    private Rng() {}

    public static long hash(long seed, long index) {
        long z = seed + index * 0x9E3779B97F4A7C15L;
        z = (z ^ (z >>> 30)) * 0xBF58476D1CE4E5B9L;
        z = (z ^ (z >>> 27)) * 0x94D049BB133111EBL;
        z = z ^ (z >>> 31);
        return z;
    }

    public static double uniform01(long seed, long index) {
        long h = hash(seed, index);
        return (h >>> 11) * (1.0 / 9007199254740992.0); // 2^53
    }

    public static double sampleIn(long seed, long index, double a, double b) {
        return a + (b - a) * uniform01(seed, index);
    }
}
