#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "benchmark.hpp"
#include <string>
#include <vector>

class Logger
{
private:
    std::string filename;

public:
    explicit Logger(const std::string& filename);

    void save(
        const std::vector<BenchmarkResult>& results
    );
};

#endif
