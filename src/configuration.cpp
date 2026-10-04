#include "configuration.hpp"

#include <fstream>
#include <string>

Configuration::Configuration()
{
    iterations = 1000;
    messageSize = 1024;
}

bool Configuration::load(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return false;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::size_t pos = line.find('=');

        if (pos == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "ITERATIONS")
        {
            iterations = std::stoi(value);
        }
        else if (key == "MESSAGE_SIZE")
        {
            messageSize = std::stoi(value);
        }
    }

    if (iterations < 1)
    {
        iterations = 1;
    }

    if (messageSize < 1)
    {
        messageSize = 1;
    }

    return true;
}

int Configuration::getIterations() const
{
    return iterations;
}

int Configuration::getMessageSize() const
{
    return messageSize;
}
