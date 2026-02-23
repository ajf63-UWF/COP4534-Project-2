#include "RandomExp.h"
#include <random>
#include <cmath>
#include <stdexcept>

double GetNextRandomInterval(double avg) {
    if (avg <= 0.0) throw std::runtime_error("avg must be > 0");

    static std::mt19937 rng(std::random_device{}());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    double f = dist(rng);
    if (f <= 0.0) f = 1e-12;

    return - (1.0 / avg) * std::log(f);
}