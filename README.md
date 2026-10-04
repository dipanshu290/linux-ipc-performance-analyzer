# Linux IPC Performance Analyzer

## Overview

Linux IPC Performance Analyzer is a C++17 Linux application that compares three inter-process communication mechanisms:

- Anonymous pipes
- POSIX message queues
- POSIX shared memory with a POSIX semaphore

The program transfers the same amount of data through each mechanism and measures elapsed time and throughput.

## Requirements

- Linux
- g++
- make
- C++17
- POSIX IPC support

## Build

```bash
make
```

## Run

```bash
./ipc_analyzer
```

## Configuration

Edit `config/benchmark.conf`:

```text
ITERATIONS=1000
MESSAGE_SIZE=1024
```

## Output

Results are printed to the terminal and saved to:

```text
results/results.txt
```

## Experimental Results

The benchmark was executed with 1000 iterations using message sizes of 1 KB, 4 KB and 8 KB.

| Message Size | Pipe (MB/s) | POSIX Message Queue (MB/s) | Shared Memory (MB/s) |
| ------------ | ----------- | -------------------------- | -------------------- |
| 1 KB         | 5.70        | 5.77                       | 11.27                |
| 4 KB         | 22.30       | 21.15                      | 23.99                |
| 8 KB         | 40.35       | 35.55                      | 51.35                |

In these tests, shared memory achieved the highest throughput for all three tested message sizes.

The measured performance values are specific to the Linux/WSL environment used during testing and may vary depending on system load and hardware configuration.

## Project Structure

```text
linux-ipc-performance-analyzer/
├── README.md
├── Makefile
├── include/
│   ├── benchmark.hpp
│   ├── configuration.hpp
│   ├── logger.hpp
│   ├── pipe_test.hpp
│   ├── message_queue_test.hpp
│   └── shared_memory_test.hpp
├── src/
│   ├── main.cpp
│   ├── benchmark.cpp
│   ├── configuration.cpp
│   ├── logger.cpp
│   ├── pipe_test.cpp
│   ├── message_queue_test.cpp
│   └── shared_memory_test.cpp
├── config/
│   └── benchmark.conf
└── results/
    ├── results.txt
    ├── results_1KB.txt
    ├── results_4KB.txt
    └── results_8KB.txt
```

## Linux Concepts Used

- `fork()`
- `pipe()`
- POSIX message queues
- POSIX shared memory
- POSIX semaphores
- `mmap()`
- `read()` and `write()`
- process synchronization
- file descriptors
- IPC performance measurement

## Conclusion

Linux IPC Performance Analyzer demonstrates the implementation and performance comparison of multiple Linux IPC mechanisms.

Based on the experiments performed, shared memory achieved the highest measured throughput for the tested 1 KB, 4 KB and 8 KB message sizes.

The project provides practical experience with Linux processes, POSIX IPC, synchronization, memory mapping, file descriptors and system-level performance measurement.
