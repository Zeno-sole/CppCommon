/*!
    \file mpsc_ring_queue.h
    \brief Multiple producers / single consumer wait-free ring queue definition
    \author Ivan Shynkarenka
    \date 21.01.2016
    \copyright MIT License
*/

#ifndef CPPCOMMON_MPSC_RING_QUEUE_H
#define CPPCOMMON_MPSC_RING_QUEUE_H

#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
#include <vector>

#include "system/rdts.h"
#include "threads/spinlock.h"
#include "threads/spsc_ring_queue.h"

namespace CppCommon {

//! Multiple producers / single consumer wait-free ring queue
/*!
    Multiple producers / single consumer wait-free ring queue use only atomic operations to provide thread-safe
    enqueue and dequeue operations. This data structure consist of several SPSC ring queues which count is
    provided as a hardware concurrency in the constructor. All of them are randomly accessed with a RDTS
    distribution index. Consumer thread sequentially copy all the items from producer's ring queues to the
    single consumer's priority queue. All the items available in sesequential or batch mode. Ring queue size
    is limited to the capacity provided in the constructor.

    FIFO order is guaranteed!
*/
template<typename T>
class MPSCRingQueue
{
public:
    //! Default class constructor
    /*!
        \param capacity - ring queue capacity (must be a power of two)
        \param concurrency - hardware concurrency (default is std::thread::hardware_concurrency)
    */
    explicit MPSCRingQueue(int64_t capacity, int64_t concurrency = std::thread::hardware_concurrency());
    MPSCRingQueue(const MPSCRingQueue&) = delete;
    MPSCRingQueue(MPSCRingQueue&&) = default;
    ~MPSCRingQueue();

    MPSCRingQueue& operator=(const MPSCRingQueue&) = delete;
    MPSCRingQueue& operator=(MPSCRingQueue&&) = default;

    //! Get ring queue capacity
    int64_t capacity() const { return _capacity; }
    //! Get ring queue concurrency
    int64_t concurrency() const { return _concurrency; }
    //! Get ring queue size
    int64_t size() const { return producer_size() + consumer_size(); }
    //! Get the current producer ring queue size
    int64_t producer_size() const { return _producers[RDTS::current() % _concurrency]->queue.size(); }
    //! Get consumer ring queue size
    int64_t consumer_size() const { return _consumer.size(); }

    //! Enqueue an item into the ring queue (multiple producers threads method)
    /*!
        The item will be copied into the ring queue.

        \param item - item to enqueue
        \return 'true' if the item was successfully enqueue, 'false' if the ring queue is full
    */
    bool Enqueue(const T& item);

    //! Dequeue an item from the ring queue (single consumer threads method)
    /*!
        The item will be copied from the ring queue.

        \param item - item to dequeue
        \return 'true' if the item was successfully dequeue, 'false' if the ring queue is empty
    */
    bool Dequeue(T& item);

    //! Dequeue all items from the linked queue (single consumer thread method)
    /*!
        All items in the batcher will be processed by the given handler.

        \param hanlder - batch handler
        \return 'true' if all items were successfully handled, 'false' if the linked batcher is empty
    */
    bool Dequeue(const std::function<void(const T&)>& handler);

private:
    struct Item
    {
        uint64_t timestamp;
        T value;

        Item() = default;
        Item(uint64_t ts, const T& v) : timestamp(ts), value(v) {}
        friend bool operator < (const Item& item1, const Item& item2) { return item1.timestamp < item2.timestamp; }
    };

    struct Producer
    {
        SpinLock lock;
        SPSCRingQueue<Item> queue;

        Producer(int64_t capacity) : queue(capacity) {}
    };

    int64_t _capacity;
    int64_t _concurrency;
    std::vector<std::shared_ptr<Producer>> _producers;
    std::priority_queue<Item> _consumer;

    // Consume an item from the consumer's priority queue
    bool Consume(T& item);
    // Flush all available items from producers' queues into the consumer's priority queue
    void Flush();
};

} // namespace CppCommon

#include "mpsc_ring_queue.inl"

#endif //CPPCOMMON_MPSC_RING_QUEUE_H
