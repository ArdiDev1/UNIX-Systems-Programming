# A Collection of UNIX Systems Programs and Notes

I created this repository as a collection of UNIX / POSIX systems programming programs written in C, focused on low-level concurrency, memory management, and process control.

The repository consists of standalone modules that explore core operating system concepts such as threading, synchronization, inter-process communication, and performance. Each directory targets a specific systems programming topic and is designed to be largely self-contained.

---

Most programs here can be compiled using `gcc` or `g++`

Example:
```bash
gcc main.c -o main -pthread
./main
