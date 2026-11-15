public final class Functions {
    private Functions() {}

    // --- ВРЕМЕНЕН/ПРИМЕРЕН ИНДИВИДУАЛЕН ВАРИАНТ ---------------------------
    // Виж VARIANT.md. Сменя се идентично с cpp/src/function.hpp.
    // f(x) = 4 / (1 + x^2) на [0, 1]; интегралът е точно pi.
    public static double f(double x) {
        return 4.0 / (1.0 + x * x);
    }

    public static final double A = 0.0;
    public static final double B = 1.0;
    public static final double REFERENCE = 3.14159265358979323846;
    // -----------------------------------------------------------------------
}
