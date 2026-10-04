#include "pipe_test.hpp"

#include <chrono>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace
{
    bool writeAll(
        int fd,
        const char* data,
        std::size_t size
    )
    {
        std::size_t total = 0;

        while (total < size)
        {
            ssize_t count = write(
                fd,
                data + total,
                size - total
            );

            if (count <= 0)
            {
                return false;
            }

            total += static_cast<std::size_t>(count);
        }

        return true;
    }

    bool readAll(
        int fd,
        char* data,
        std::size_t size
    )
    {
        std::size_t total = 0;

        while (total < size)
        {
            ssize_t count = read(
                fd,
                data + total,
                size - total
            );

            if (count <= 0)
            {
                return false;
            }

            total += static_cast<std::size_t>(count);
        }

        return true;
    }
}

BenchmarkResult benchmarkPipe(
    int iterations,
    int messageSize
)
{
    BenchmarkResult result;
    result.name = "Pipe";
    result.messages = iterations;
    result.bytes =
        static_cast<long long>(iterations) * messageSize;
    result.milliseconds = 0.0;
    result.throughputMBps = 0.0;

    int parentToChild[2];
    int childToParent[2];

    if (pipe(parentToChild) == -1 ||
        pipe(childToParent) == -1)
    {
        return result;
    }

    pid_t child = fork();

    if (child == -1)
    {
        return result;
    }

    std::vector<char> buffer(messageSize, 'A');

    if (child == 0)
    {
        close(parentToChild[1]);
        close(childToParent[0]);

        std::vector<char> received(messageSize);

        for (int i = 0; i < iterations; i++)
        {
            if (!readAll(
                    parentToChild[0],
                    received.data(),
                    received.size()))
            {
                _exit(1);
            }

            if (!writeAll(
                    childToParent[1],
                    received.data(),
                    received.size()))
            {
                _exit(1);
            }
        }

        close(parentToChild[0]);
        close(childToParent[1]);
        _exit(0);
    }

    close(parentToChild[0]);
    close(childToParent[1]);

    auto start = std::chrono::steady_clock::now();

    bool ok = true;

    for (int i = 0; i < iterations; i++)
    {
        if (!writeAll(
                parentToChild[1],
                buffer.data(),
                buffer.size()))
        {
            ok = false;
            break;
        }

        if (!readAll(
                childToParent[0],
                buffer.data(),
                buffer.size()))
        {
            ok = false;
            break;
        }
    }

    auto end = std::chrono::steady_clock::now();

    close(parentToChild[1]);
    close(childToParent[0]);

    int status = 0;
    waitpid(child, &status, 0);

    if (!ok || !WIFEXITED(status) ||
        WEXITSTATUS(status) != 0)
    {
        return result;
    }

    result.milliseconds =
        std::chrono::duration<double, std::milli>(
            end - start
        ).count();

    if (result.milliseconds > 0.0)
    {
        result.throughputMBps =
            (result.bytes / (1024.0 * 1024.0)) /
            (result.milliseconds / 1000.0);
    }

    return result;
}
