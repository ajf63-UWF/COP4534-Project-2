#ifndef ANALYTICALMODEL_H
#define ANALYTICALMODEL_H

#include "Simulation.h"

struct AnalyticalStats {
    long double P0 = 0.0L;    // Probability of zero customers in system
    long double L = 0.0L;     // Average number of customers in system
    long double W = 0.0L;     // Average time a customer spends in system
    long double Lq = 0.0L;    // Average number of customers in queue
    long double Wq = 0.0L;    // Average time a customer spends waiting in queue
    long double rho = 0.0L;   // Server utilization
    bool stable = true;        // Whether the system is stable (rho < 1)
};

AnalyticalStats ComputeAnalyticalModel(const SimConfig& cfg);

#endif // ANALYTICALMODEL_H