#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <string>

class Configuration
{
private:
    int iterations;
    int messageSize;

public:
    Configuration();

    bool load(const std::string& filename);

    int getIterations() const;
    int getMessageSize() const;
};

#endif
