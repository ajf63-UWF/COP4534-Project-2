#include "Simulation.h"
#include "Event.h"
#include "MinHeap.h"
#include "FIFOQueue.h"
#include "RandomExp.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

struct Customer {
    int id = -1;
    double arrivalTime = 0.0;
};

static int acquireFreeServer(std::vector<bool>& busy) {
    for (int i = 0; i < (int)busy.size(); i++) {
        if (!busy[i]) {
            busy[i] = true;
            return i;
        }
    }
    return -1;
}

static void releaseServer(std::vector<bool>& busy, int serverID) {
    if (serverID >= 0 && serverID < (int)busy.size()) {
        busy[serverID] = false;
    }
}

bool ReadConfig(const std::string& filename, SimConfig& cfg) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "ERROR: couldn't open file " << filename << "\n";
        return false;
    }

    if (!(in >> cfg.lambda >> cfg.mu >> cfg.M >> cfg.N)) {
        std::cerr << "ERROR: invalid format in " << filename 
        << " (expected 4 numbers: lambda, mu, M, N)\n";
        return false;
    }

    return true;
}

SimStats RunSimulation(const SimConfig& cfg) {
    if (cfg.lambda <= 0 || cfg.mu <= 0 || cfg.M <= 0 || cfg.N <= 0) {
        throw std::runtime_error("Invalid config values");
    }

    const int PQ_CAP = 200;

    MinHeap<Event> pq(PQ_CAP);
    FIFOQueue<Customer> waitQ;
    std::vector<bool> serverBusy(cfg.M, false);

    SimStats stats;

    double lastEventTime = 0.0;
    int numInSystem = 0; 

    int arrivalsGenerated = 0;
    int nextCustomerID = 0;
    double nextArrivalTime = 0.0;

    std::vector<double> arrivalTime(cfg.N, -1.0);
    std::vector<double> serviceStart(cfg.N, -1.0);
    std::vector<double> serviceDur(cfg.N, 0.0);

    auto addArrival = [&]() {
        if (arrivalsGenerated >= cfg.N) return;
        if (nextCustomerID >= cfg.N) {
            throw std::runtime_error("BUG: nextCustomerID >= N while generating arrivals");
        }

        nextArrivalTime += GetNextRandomInterval(cfg.lambda);

        Event e;
        e.time = nextArrivalTime;
        e.type = EventType::ARRIVAL;
        e.customerID = nextCustomerID++;
        e.serverID = -1;

        pq.push(e);
        arrivalsGenerated++;
    };

    while ((int)pq.size() < PQ_CAP && arrivalsGenerated < cfg.N) {
        addArrival();
    }

    while (stats.customersServed < cfg.N) {
        if (pq.empty()) throw std::runtime_error("PQ empty before finishing simulation");

        if ((int)pq.size() <= cfg.M + 1 && arrivalsGenerated < cfg.N) {
            while ((int)pq.size() < PQ_CAP && arrivalsGenerated < cfg.N) {
                addArrival();
            }
        }

        Event ev = pq.pop();

        double dt = ev.time - lastEventTime;
        if (dt < 0) throw std::runtime_error("BUG: time went backwards");
        if (numInSystem == 0) stats.idleTime += dt;
        lastEventTime = ev.time;
        stats.endTime = ev.time;

        if (ev.type == EventType::ARRIVAL) {
            const int cid = ev.customerID;
            if (cid < 0 || cid >= cfg.N) throw std::runtime_error("ARRIVAL invalid customerID");

            numInSystem++;

            arrivalTime[cid] = ev.time;

            Customer c;
            c.id = cid;
            c.arrivalTime = ev.time;

            int sid = acquireFreeServer(serverBusy);
            if (sid != -1) {
                serviceStart[cid] = ev.time;
                serviceDur[cid] = GetNextRandomInterval(cfg.mu);

                Event dep;
                dep.time = ev.time + serviceDur[cid];
                dep.type = EventType::DEPARTURE;
                dep.serverID = sid;
                dep.customerID = cid;
                pq.push(dep);
            } else {
                waitQ.enqueue(c);
                stats.customersWaited++;

                if ((int)waitQ.size() > stats.maxQueueLen) {
                    stats.maxQueueLen = (int)waitQ.size();
                }
            }

        } else { // DEPARTURE
            const int cid = ev.customerID;
            if (cid < 0 || cid >= cfg.N) throw std::runtime_error("DEPARTURE invalid customerID");
            if (ev.serverID < 0 || ev.serverID >= cfg.M) throw std::runtime_error("DEPARTURE invalid serverID");

            if (arrivalTime[cid] < 0.0 || serviceStart[cid] < 0.0) {
                throw std::runtime_error("Missing arrival/serviceStart for departed customer");
            }

            numInSystem--;

            const double wait = serviceStart[cid] - arrivalTime[cid];
            stats.totalWaitTime += wait;
            stats.totalServiceTime += serviceDur[cid];
            stats.customersServed++;

            // free server
            releaseServer(serverBusy, ev.serverID);

            // if someone waiting, start immediately on the freed server
            if (!waitQ.empty()) {
                Customer next = waitQ.dequeue();
                if (next.id < 0 || next.id >= cfg.N) throw std::runtime_error("Dequeued invalid customer id");

                const int sid = ev.serverID;
                serverBusy[sid] = true;

                serviceStart[next.id] = ev.time;
                serviceDur[next.id] = GetNextRandomInterval(cfg.mu);

                Event dep;
                dep.time = ev.time + serviceDur[next.id];
                dep.type = EventType::DEPARTURE;
                dep.serverID = sid;
                dep.customerID = next.id;
                pq.push(dep);
            }
        }
    }

    return stats;
}