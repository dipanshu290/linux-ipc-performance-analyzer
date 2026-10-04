#include "benchmark.hpp"
#include "message_queue_test.hpp"
#include "pipe_test.hpp"
#include "shared_memory_test.hpp"

#include <iomanip>
#include <iostream>
#include <string>

Benchmark::Benchmark(
    int iterations,
    int messageSize
)
{
    this->iterations = iterations;
    this->messageSize = messageSize;
}

BenchmarkResult Benchmark::runPipe()
{
    return benchmarkPipe(
        iterations,
        messageSize
    );
}

BenchmarkResult Benchmark::runMessageQueue()
{
    return benchmarkMessageQueue(
        iterations,
        messageSize
    );
}

BenchmarkResult Benchmark::runSharedMemory()
{
    return benchmarkSharedMemory(
        iterations,
        messageSize
    );
}

void Benchmark::printResults(
    const std::vector<BenchmarkResult>& results
)
{
    std::cout << std::endl;

    std::cout << std::left
              << std::setw(22) << "IPC"
              << std::setw(12) << "Messages"
              << std::setw(15) << "Bytes"
              << std::setw(15) << "Time(ms)"
              << "Throughput(MB/s)"
              << std::endl;

    std::cout << std::string(80, '-')
              << std::endl;

    for (const auto& result : results)
    {
        std::cout << std::left
                  << std::setw(22) << result.name
                  << std::setw(12) << result.messages
                  << std::setw(15) << result.bytes
                  << std::setw(15)
                  << std::fixed
                  << std::setprecision(3)
                  << result.milliseconds
                  << std::setprecision(2)
                  << result.throughputMBps
                  << std::endl;
    }

    std::cout << std::endl;
}
