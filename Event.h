#ifndef EVENT_H
#define EVENT_H

enum class EventType { ARRIVAL, DEPARTURE };

struct Event {
    double time = 0.0;
    EventType type = EventType::ARRIVAL;

    int serverID = -1;
    int customerID = -1;

    static bool less(const Event& a, const Event& b) {
        if (a.time < b.time) return true;
        if (a.time > b.time) return false;

        if (a.type != b.type) {
            return (a.type == EventType::DEPARTURE);
        }
        return a.customerID < b.customerID;
    }
};

#endif // EVENT_H