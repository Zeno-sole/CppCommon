//
// Created by Ivan Shynkarenka on 15.01.2016.
//

#include "catch.hpp"

#include "threads/ring_queue.h"

using namespace CppCommon;

TEST_CASE("Wait-free ring queue", "[CppCommon][Threads]")
{
    RingQueue<int, 4> queue;

    REQUIRE(queue.Enqueue(0));
    REQUIRE(queue.Enqueue(1));
    REQUIRE(queue.Enqueue(2));
    REQUIRE(!queue.Enqueue(3));

    int v = -1;

    REQUIRE((queue.Dequeue(v) && (v == 0)));
    REQUIRE((queue.Dequeue(v) && (v == 1)));

    REQUIRE(queue.Enqueue(3));
    REQUIRE(queue.Enqueue(4));
    REQUIRE(!queue.Enqueue(5));

    REQUIRE((queue.Dequeue(v) && (v == 2)));
    REQUIRE((queue.Dequeue(v) && (v == 3)));
    REQUIRE((queue.Dequeue(v) && (v == 4)));
    REQUIRE(!queue.Dequeue(v));

    REQUIRE(queue.Enqueue(5));

    REQUIRE((queue.Dequeue(v) && (v == 5)));
    REQUIRE(!queue.Dequeue(v));
}