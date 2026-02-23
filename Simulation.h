#ifndef SIMULATION_H
#define SIMULATION_H

#include <string> 

struct SimConfig {
    double lambda = 0.0;
    double mu = 0.0;
    int M = 1;
    int N = 0;
};

struct SimStats {
    int customersServed = 0;
    double totalWaitTime = 0.0;
    double totalServiceTime = 0.0;
    int maxQueueLen = 0;
    double endTime = 0.0;
    double idleTime = 0.0;
    int customersWaited = 0;
};

bool ReadConfig(const std::string& filename, SimConfig& cfg);
SimStats RunSimulation(const SimConfig& cfg);

#endif // SIMULATION_H