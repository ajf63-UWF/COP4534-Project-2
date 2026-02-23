#include "Simulation.h"
#include "AnalyticalModel.h"
#include <iostream>
#include <iomanip>

static void runFile(const std::string& filename) {
    SimConfig cfg;
    if (!ReadConfig(filename, cfg)) {
        std::cerr << "Failed to read config from " << filename << "\n";
        return;
    }

    // Run simulation
    SimStats sim = RunSimulation(cfg);

    // Analytical model
    AnalyticalStats a = ComputeAnalyticalModel(cfg);

    const double Wq_sim = sim.totalWaitTime / sim.customersServed;
    const double S_sim  = sim.totalServiceTime / sim.customersServed;
    const double W_sim  = Wq_sim + S_sim;

    const double P0_sim  = (sim.endTime > 0.0) ? (sim.idleTime / sim.endTime) : 0.0;
    const double rho_sim = (sim.endTime > 0.0) ? (sim.totalServiceTime / (cfg.M * sim.endTime)) : 0.0;
    const double pWait_sim = (sim.customersServed > 0) ? ((double)sim.customersWaited / sim.customersServed) : 0.0;

    std::cout << "\n===== " << filename << " =====\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "lambda=" << cfg.lambda
              << " mu=" << cfg.mu
              << " M=" << cfg.M
              << " N=" << cfg.N << "\n";

    // Analytical output
    std::cout << "\n--- Analytical Model (M/M/M) ---\n";
    if (!a.stable) {
        std::cout << "UNSTABLE: lambda >= M*mu, analytical measures diverge.\n";
        std::cout << "rho = " << (double)a.rho << "\n";
    } else {
        std::cout << "P0  = " << (double)a.P0  << "\n";
        std::cout << "L   = " << (double)a.L   << "\n";
        std::cout << "W   = " << (double)a.W   << "\n";
        std::cout << "Lq  = " << (double)a.Lq  << "\n";
        std::cout << "Wq  = " << (double)a.Wq  << "\n";
        std::cout << "rho = " << (double)a.rho << "\n";
    }

    // Simulation output
    std::cout << "\n--- Simulation Measures ---\n";
    std::cout << "P0 (sim)   = " << P0_sim << "\n";
    std::cout << "W  (sim)   = " << W_sim  << "\n";
    std::cout << "Wq (sim)   = " << Wq_sim << "\n";
    std::cout << "rho (sim)  = " << rho_sim << "\n";
    std::cout << "P(wait)    = " << pWait_sim << "\n";
}

int main() {
    runFile("test1.txt");
    runFile("test2.txt");

    return 0;
}