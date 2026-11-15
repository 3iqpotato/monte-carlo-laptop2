import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public final class MonteCarlo {
    private MonteCarlo() {}

    public static double estimateSequential(long seed, long n) {
        double sum = 0.0;
        for (long i = 0; i < n; i++) {
            double x = Rng.sampleIn(seed, i, Functions.A, Functions.B);
            sum += Functions.f(x);
        }
        return (Functions.B - Functions.A) * sum / n;
    }

    /**
     * Явно разпределение: [0, n) се дели на {@code threads} последователни блока
     * (остатъкът се раздава по 1 на първите блокове), всеки блок е Callable в
     * ExecutorService, а частичните суми се събират в главната нишка през Future.
     * Аналог на OpenMP schedule(static) + reduction(+:sum) в C++ версията.
     * Пулът се създава извън измерваното време — виж Main.
     */
    public static double estimateParallel(ExecutorService pool, long seed, long n, int threads)
            throws InterruptedException, ExecutionException {
        List<Future<Double>> futures = new ArrayList<>(threads);
        long chunk = n / threads;
        long remainder = n % threads;
        long start = 0;
        for (int t = 0; t < threads; t++) {
            final long from = start;
            final long to = from + chunk + (t < remainder ? 1 : 0);
            start = to;
            futures.add(pool.submit(() -> {
                double partial = 0.0;
                for (long i = from; i < to; i++) {
                    double x = Rng.sampleIn(seed, i, Functions.A, Functions.B);
                    partial += Functions.f(x);
                }
                return partial;
            }));
        }
        double sum = 0.0;
        for (Future<Double> f : futures) {
            sum += f.get();
        }
        return (Functions.B - Functions.A) * sum / n;
    }
}
