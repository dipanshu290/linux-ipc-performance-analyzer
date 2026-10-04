#include "logger.hpp"

#include <fstream>

Logger::Logger(const std::string& filename)
{
    this->filename = filename;
}

void Logger::save(
    const std::vector<BenchmarkResult>& results
)
{
    std::ofstream file(filename);

    if (!file.is_open())
    {
        return;
    }

    file << "Linux IPC Performance Analyzer"
         << std::endl;
    file << std::endl;

    file << "IPC,Messages,Bytes,Time(ms),Throughput(MB/s)"
         << std::endl;

    for (const auto& result : results)
    {
        file << result.name << ","
             << result.messages << ","
             << result.bytes << ","
             << result.milliseconds << ","
             << result.throughputMBps
             << std::endl;
    }
}
