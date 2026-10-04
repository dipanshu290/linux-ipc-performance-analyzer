#include "benchmark.hpp"
#include "configuration.hpp"
#include "logger.hpp"

#include <iostream>
#include <vector>

int main()
{
    Configuration config;

    if (!config.load("config/benchmark.conf"))
    {
        std::cerr
            << "Could not load configuration"
            << std::endl;
        return 1;
    }

    std::cout
        << "Linux IPC Performance Analyzer"
        << std::endl;

    std::cout
        << "Iterations: "
        << config.getIterations()
        << std::endl;

    std::cout
        << "Message size: "
        << config.getMessageSize()
        << " bytes"
        << std::endl;

    Benchmark benchmark(
        config.getIterations(),
        config.getMessageSize()
    );

    std::vector<BenchmarkResult> results;

    std::cout << std::endl
              << "Running pipe test..."
              << std::endl;

    results.push_back(benchmark.runPipe());

    std::cout << "Running message queue test..."
              << std::endl;

    results.push_back(
        benchmark.runMessageQueue()
    );

    std::cout << "Running shared memory test..."
              << std::endl;

    results.push_back(
        benchmark.runSharedMemory()
    );

    Benchmark::printResults(results);

    Logger logger("results/results.txt");
    logger.save(results);

    std::cout
        << "Results saved to results/results.txt"
        << std::endl;

    return 0;
}
