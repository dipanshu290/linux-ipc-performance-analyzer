#include "message_queue_test.hpp"

#include <chrono>
#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <string>

BenchmarkResult benchmarkMessageQueue(
    int iterations,
    int messageSize
)
{
    BenchmarkResult result;
    result.name = "POSIX Message Queue";
    result.messages = iterations;
    result.bytes =
        static_cast<long long>(iterations) * messageSize;
    result.milliseconds = 0.0;
    result.throughputMBps = 0.0;

    std::string sendName =
        "/ipc_send_" + std::to_string(getpid());

    std::string receiveName =
        "/ipc_receive_" + std::to_string(getpid());

    mq_attr attributes{};
    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = messageSize;

    mqd_t sendQueue = mq_open(
        sendName.c_str(),
        O_CREAT | O_RDWR,
        0600,
        &attributes
    );

    mqd_t receiveQueue = mq_open(
        receiveName.c_str(),
        O_CREAT | O_RDWR,
        0600,
        &attributes
    );

    if (sendQueue == static_cast<mqd_t>(-1) ||
        receiveQueue == static_cast<mqd_t>(-1))
    {
        if (sendQueue != static_cast<mqd_t>(-1))
        {
            mq_close(sendQueue);
            mq_unlink(sendName.c_str());
        }

        if (receiveQueue != static_cast<mqd_t>(-1))
        {
            mq_close(receiveQueue);
            mq_unlink(receiveName.c_str());
        }

        return result;
    }

    pid_t child = fork();

    if (child == -1)
    {
        mq_close(sendQueue);
        mq_close(receiveQueue);
        mq_unlink(sendName.c_str());
        mq_unlink(receiveName.c_str());
        return result;
    }

    std::vector<char> buffer(messageSize, 'B');

    if (child == 0)
    {
        std::vector<char> received(messageSize);

        for (int i = 0; i < iterations; i++)
        {
            ssize_t count = mq_receive(
                sendQueue,
                received.data(),
                received.size(),
                nullptr
            );

            if (count != messageSize)
            {
                _exit(1);
            }

            if (mq_send(
                    receiveQueue,
                    received.data(),
                    received.size(),
                    0) == -1)
            {
                _exit(1);
            }
        }

        mq_close(sendQueue);
        mq_close(receiveQueue);
        _exit(0);
    }

    auto start = std::chrono::steady_clock::now();

    bool ok = true;

    for (int i = 0; i < iterations; i++)
    {
        if (mq_send(
                sendQueue,
                buffer.data(),
                buffer.size(),
                0) == -1)
        {
            ok = false;
            break;
        }

        ssize_t count = mq_receive(
            receiveQueue,
            buffer.data(),
            buffer.size(),
            nullptr
        );

        if (count != messageSize)
        {
            ok = false;
            break;
        }
    }

    auto end = std::chrono::steady_clock::now();

    if (!ok)
    {
        kill(child, SIGTERM);
    }

    waitpid(child, nullptr, 0);

    mq_close(sendQueue);
    mq_close(receiveQueue);
    mq_unlink(sendName.c_str());
    mq_unlink(receiveName.c_str());

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
