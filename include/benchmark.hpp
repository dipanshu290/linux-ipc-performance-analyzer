#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <string>
#include <vector>

struct BenchmarkResult
{
    std::string name;
    long long messages;
    long long bytes;
    double milliseconds;
    double throughputMBps;
};

class Benchmark
{
private:
    int iterations;
    int messageSize;

public:
    Benchmark(int iterations, int messageSize);

    BenchmarkResult runPipe();
    BenchmarkResult runMessageQueue();
    BenchmarkResult runSharedMemory();

    static void printResults(
        const std::vector<BenchmarkResult>& results
    );
};

#endif
