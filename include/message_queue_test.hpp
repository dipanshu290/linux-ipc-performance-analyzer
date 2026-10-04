#ifndef MESSAGE_QUEUE_TEST_HPP
#define MESSAGE_QUEUE_TEST_HPP

#include "benchmark.hpp"

BenchmarkResult benchmarkMessageQueue(
    int iterations,
    int messageSize
);

#endif
