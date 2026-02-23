#include "AnalyticalModel.h"
#include <cmath>

static long double factorial_int(int n) {
    long double f = 1.0L;
    for (int i = 2; i <= n; i++) f *= (long double)i;
    return f;
}

AnalyticalStats ComputeAnalyticalModel(const SimConfig& cfg) {
    AnalyticalStats a;

    const long double lambda = (long double)cfg.lambda;
    const long double mu = (long double)cfg.mu;
    const int M = cfg.M;

    a.rho = lambda / ((long double)M * mu);
    a.stable = (a.rho < 1.0L);
    if (!a.stable) return a;

    const long double x = lambda / mu;

    // Compute P0
    long double sum = 0.0L;
    for (int i = 0; i <= M - 1; i++) {
        sum += std::pow(x, (long double)i) / factorial_int(i);
    }

    long double tail = 
        (std::pow(x, (long double)M) / factorial_int(M)) * 
        (((long double)M * mu) / (((long double)M * mu) - lambda));

    a.P0 = 1.0L / (sum + tail);

    // L
    a.L = (lambda * mu * std::pow(x, (long double)M) /
        (factorial_int(M - 1) * std::pow(((long double)M * mu - lambda), 2.0L))) * a.P0
        + (lambda / mu);

    // W
    a.W = a.L / lambda;

    // Lq
    a.Lq = a.L - (lambda / mu);

    // Wq
    a.Wq = a.Lq / lambda;

    return a;
}