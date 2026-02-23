#ifndef MINHEAP_H
#define MINHEAP_H

#include <vector>
#include <stdexcept>

template <typename T>
class MinHeap {
public:
    explicit MinHeap(size_t capacity = 0) {
        if (capacity > 0) data.reserve(capacity);
    }

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }

    const T& top() const {
        if (data.empty()) throw std::runtime_error("Heap is empty");
        return data[0];
    }

    void push(const T& item) {
        data.push_back(item);
        siftUp(data.size() - 1);
    }

    T pop() {
        if (data.empty()) throw std::runtime_error("Heap is empty");
        T minItem = data[0];
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) siftDown(0);
        return minItem;
    }

private:
    std::vector<T> data;

    static bool less(const T& a, const T& b) {
        return T::less(a, b);
    }

    void siftUp(size_t i) {
        while (i > 0) {
            size_t p = (i -1) / 2;
            if (!less(data[i], data[p])) break;
            std::swap(data[i], data[p]);
            i = p;
        }
    }

    void siftDown(size_t index) {
        size_t i = index;
        size_t n = data.size();
        while (true) {
            size_t l = 2 * i + 1;
            size_t r = 2 * i + 2;
            size_t smallest = i;

            if (l < data.size() && less(data[l], data[smallest])) smallest = l;
            if (r < data.size() && less(data[r], data[smallest])) smallest = r;

            if (smallest == i) break;
            std::swap(data[i], data[smallest]);
            i = smallest;
        }
    }
};

#endif // MINHEAP_H