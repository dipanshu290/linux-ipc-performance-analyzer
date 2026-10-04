#include "shared_memory_test.hpp"

#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <string>

namespace
{
    struct SharedData
    {
        sem_t parentReady;
        sem_t childReady;
        char data[1];
    };
}

BenchmarkResult benchmarkSharedMemory(
    int iterations,
    int messageSize
)
{
    BenchmarkResult result;
    result.name = "Shared Memory";
    result.messages = iterations;
    result.bytes =
        static_cast<long long>(iterations) * messageSize;
    result.milliseconds = 0.0;
    result.throughputMBps = 0.0;

    std::string name =
        "/ipc_shm_" + std::to_string(getpid());

    std::size_t totalSize =
        sizeof(SharedData) + messageSize - 1;

    int fd = shm_open(
        name.c_str(),
        O_CREAT | O_RDWR,
        0600
    );

    if (fd == -1)
    {
        return result;
    }

    if (ftruncate(fd, totalSize) == -1)
    {
        close(fd);
        shm_unlink(name.c_str());
        return result;
    }

    void* memory = mmap(
        nullptr,
        totalSize,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );

    close(fd);

    if (memory == MAP_FAILED)
    {
        shm_unlink(name.c_str());
        return result;
    }

    SharedData* shared =
        static_cast<SharedData*>(memory);

    if (sem_init(&shared->parentReady, 1, 0) == -1 ||
        sem_init(&shared->childReady, 1, 0) == -1)
    {
        munmap(memory, totalSize);
        shm_unlink(name.c_str());
        return result;
    }

    pid_t child = fork();

    if (child == -1)
    {
        sem_destroy(&shared->parentReady);
        sem_destroy(&shared->childReady);
        munmap(memory, totalSize);
        shm_unlink(name.c_str());
        return result;
    }

    std::vector<char> buffer(messageSize, 'C');

    if (child == 0)
    {
        std::vector<char> received(messageSize);

        for (int i = 0; i < iterations; i++)
        {
            if (sem_wait(&shared->parentReady) == -1)
            {
                _exit(1);
            }

            std::memcpy(
                received.data(),
                shared->data,
                messageSize
            );

            std::memcpy(
                shared->data,
                received.data(),
                messageSize
            );

            if (sem_post(&shared->childReady) == -1)
            {
                _exit(1);
            }
        }

        munmap(memory, totalSize);
        _exit(0);
    }

    auto start = std::chrono::steady_clock::now();

    bool ok = true;

    for (int i = 0; i < iterations; i++)
    {
        std::memcpy(
            shared->data,
            buffer.data(),
            messageSize
        );

        if (sem_post(&shared->parentReady) == -1)
        {
            ok = false;
            break;
        }

        if (sem_wait(&shared->childReady) == -1)
        {
            ok = false;
            break;
        }

        std::memcpy(
            buffer.data(),
            shared->data,
            messageSize
        );
    }

    auto end = std::chrono::steady_clock::now();

    if (!ok)
    {
        kill(child, SIGTERM);
    }

    waitpid(child, nullptr, 0);

    sem_destroy(&shared->parentReady);
    sem_destroy(&shared->childReady);
    munmap(memory, totalSize);
    shm_unlink(name.c_str());

    if (!ok)
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
