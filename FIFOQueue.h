#ifndef FIFOQUEUE_H
#define FIFOQUEUE_H

#include <deque>
#include <stdexcept>

template <typename T>
class FIFOQueue {
public: 
    bool empty() const { return q.empty(); }
    size_t size() const { return q.size(); }

    void enqueue(const T& item) {
        q.push_back(item);
    }

    T dequeue() {
        if (q.empty()) throw std::runtime_error("FIFOQueue is empty");
        T front = q.front();
        q.pop_front();
        return front;
    }

private:
    std::deque<T> q;
};

#endif // FIFOQUEUE_H