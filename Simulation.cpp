#include "Simulation.h"
#include "Event.h"
#include "MinHeap.h"
#include "FIFOQueue.h"
#include "RandomExp.h"

#include <fstream>
#include <vector>
#include <valarray>
#include <stdexcept>

struct Customer {
    int id = -1;
    double arrivalTime = 0.0;
};

static int acquierFreeServer(std::vector<bool>& busy) {
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
    std::ifstream file(filename);
    if (!file) return false;

    file >> cfg.lambda;
    file >> cfg.mu;
    file >> cfg.M;
    file >> cfg.N;

    return file.good();
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

    int arrivalsGenerated = 0;
    int nextCustomerID = 0;
    double nextArrivalTime = 0.0;

    std::vector<double> arrivalTime(cfg.N, -1.0);
    std::vector<double> serviceStart(cfg.N, -1.0);
    std::valarray<double> serviceDur(cfg.N, 0.0);

    auto addArrival = [&]() {
            nextArrivalTime += GetNextRandomInterval(cfg.lambda);

            int cid = nextCustomerID;
            Event e;
            e.time = nextArrivalTime;
            e.type = EventType::ARRIVAL;
            e.customerID = nextCustomerID++;

            pq.push(e);
            arrivalsGenerated++;
    };

    while ((int)pq.size() < PQ_CAP && arrivalsGenerated < cfg.N) {
        addArrival();
    }

    while (stats.customersServed < cfg.N) {
        if (pq.empty()) throw std::runtime_error("PQ empty before finishign simulation");

        if((int)pq.size() <= cfg.M + 1 && arrivalsGenerated < cfg.N) {
            while ((int)pq.size() < PQ_CAP && arrivalsGenerated < cfg.N) {
                addArrival();
            }
        }

        Event ev = pq.pop();
        stats.endTime = ev.time;

        if (ev.type == EventType::ARRIVAL) {
            Customer c;
            c.id = ev.customerID;
            c.arrivalTime = ev.time;

            int sid = acquierFreeServer(serverBusy);
            if (sid != -1) {
                //Starts immediately
                serviceStart[c.id] = ev.time;
                serviceDur[c.id] = GetNextRandomInterval(cfg.mu);

                Event dep;
                dep.time = ev.time + serviceDur[c.id];
                dep.type = EventType::DEPARTURE;
                dep.serverID = sid;
                dep.customerID = c.id;
                pq.push(dep);
            } else {
                waitQ.enqueue(c);
                if ((int)waitQ.size() > stats.maxQueueLen) {
                    stats.maxQueueLen = waitQ.size();
                }
            }
        } else {
            int cid = ev.customerID;

            releaseServer(serverBusy, ev.serverID);

            double wait = serviceStart[cid] - arrivalTime[cid];
            stats.totalWaitTime += wait;
            stats.totalServiceTime += serviceDur[cid];
            stats.customersServed++;

            if (!waitQ.empty()) {
                Customer next = waitQ.dequeue();
                int sid = acquierFreeServer(serverBusy);
                if (sid == -1) {
                    sid = 0;
                    serverBusy[0] = true;
                }

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