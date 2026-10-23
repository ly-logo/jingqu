#pragma once

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>
#include <utility>

namespace scenicflow {

// Thread-safe bounded FIFO used by the pipeline stages.
// std::priority_queue and friends are not thread safe, so the queue is wrapped
// here with a mutex and a hard capacity limit (design constraint).
template <typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(size_t maxSize = 1000) : m_maxSize(maxSize) {}

    // Returns false when the queue is full (callers may drop the oldest entry).
    bool tryPush(const T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_items.size() >= m_maxSize) return false;
        m_items.push_back(value);
        m_cv.notify_one();
        return true;
    }

    // Never blocks for longer than the caller can afford: used by tick loops.
    std::optional<T> tryPop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_items.empty()) return std::nullopt;
        T value = std::move(m_items.front());
        m_items.pop_front();
        return value;
    }

    std::optional<T> waitPopFor(int timeoutMs) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (!m_cv.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                           [this] { return !m_items.empty() || m_stopped; })) {
            return std::nullopt;
        }
        if (m_items.empty()) return std::nullopt;
        T value = std::move(m_items.front());
        m_items.pop_front();
        return value;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stopped = true;
        }
        m_cv.notify_all();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_items.clear();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_items.size();
    }

private:
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::deque<T> m_items;
    size_t m_maxSize;
    bool m_stopped = false;
};

} // namespace scenicflow
