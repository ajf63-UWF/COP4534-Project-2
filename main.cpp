#include "Simulation.h"
#include <iostream>
#include <iomanip>

static void runFile(const std::string& filename) {
    SimConfig cfg;
    if (!ReadConfig(filename, cfg)) {
        std::cerr << "Failed to read config from " << filename << "\n";
        return;
    }

    SimStats stats = RunSimulation(cfg);

    double avgWait = stats.totalWaitTime / stats.customersServed;
    double avgService = stats.totalServiceTime / stats.customersServed;

    std::cout   << "===== " << filename << " ====\n";
    std::cout   << "lambda=" << cfg.lambda
                << " mu=" << cfg.mu
                << " M=" << cfg.M
                << " N=" << cfg.N << "\n";
    std::cout   << "customers served: " << stats.customersServed << "\n";
    std::cout   << std::fixed << std::setprecision(6);
    std::cout   << "avg wait time: " << avgWait << "\n";
    std::cout   << "avg service time: " << avgService << "\n";
    std::cout   << "max queue length: " << stats.maxQueueLen << "\n";
    std::cout   << "simulation end time: " << stats.endTime << "\n";
}

int main() {
    runFile("test1.txt");
    runFile("test2.txt");

    return 0;
}