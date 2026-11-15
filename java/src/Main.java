import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public final class Main {
    public static void main(String[] args) throws Exception {
        Map<String, String> opt = new HashMap<>();
        for (int i = 0; i < args.length; i++) {
            if (!args[i].startsWith("--") || i + 1 >= args.length) {
                System.err.println("Bad argument: " + args[i]);
                System.exit(1);
            }
            opt.put(args[i].substring(2), args[++i]);
        }

        String mode = opt.getOrDefault("mode", "seq");
        long n = Long.parseLong(opt.getOrDefault("n", "1000000"));
        int threads = Integer.parseInt(opt.getOrDefault("threads", "1"));
        long seed = Long.parseLong(opt.getOrDefault("seed", "42"));
        int repeats = Integer.parseInt(opt.getOrDefault("repeats", "1"));
        int warmup = Integer.parseInt(opt.getOrDefault("warmup", "1"));
        String machine = opt.getOrDefault("machine", "unknown");

        if (mode.equals("verify")) {
            System.out.println("index,hash_u64,uniform01_bits");
            for (long i = 0; i < 10; i++) {
                long bits = Double.doubleToRawLongBits(Rng.uniform01(seed, i));
                System.out.println(i + "," + Long.toUnsignedString(Rng.hash(seed, i)) + ","
                        + String.format("%016x", bits));
            }
            return;
        }
        if (!mode.equals("seq") && !mode.equals("par")) {
            System.err.println("Unknown mode: " + mode);
            System.exit(1);
        }

        ExecutorService pool = mode.equals("par") ? Executors.newFixedThreadPool(threads) : null;

        System.out.println("language,mode,n,threads,seed,repeat_index,time_seconds,estimate,"
                + "abs_error,rel_error,samples_per_sec,machine");
        try {
            // Неизмервани загряващи пускания: JIT компилация и стартиране на нишките в пула.
            double sink = 0.0;
            for (int w = 0; w < warmup; w++) {
                sink += run(pool, seed, n, threads);
            }
            if (Double.isNaN(sink)) System.err.println("warmup NaN");

            for (int r = 0; r < repeats; r++) {
                long t0 = System.nanoTime();
                double estimate = run(pool, seed, n, threads);
                long t1 = System.nanoTime();
                double seconds = (t1 - t0) / 1e9;

                double absErr = Math.abs(estimate - Functions.REFERENCE);
                double relErr = absErr / Functions.REFERENCE;
                double sps = seconds > 0 ? n / seconds : 0.0;

                System.out.println(String.format(java.util.Locale.ROOT,
                        "java,%s,%d,%d,%d,%d,%.9f,%.12f,%.12e,%.12e,%.6f,%s",
                        mode, n, threads, seed, r, seconds, estimate, absErr, relErr, sps, machine));
            }
        } finally {
            if (pool != null) pool.shutdown();
        }
    }

    private static double run(ExecutorService pool, long seed, long n, int threads) throws Exception {
        return pool == null
                ? MonteCarlo.estimateSequential(seed, n)
                : MonteCarlo.estimateParallel(pool, seed, n, threads);
    }
}
