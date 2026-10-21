# Producer-Consumer Bounded-Buffer Simulation Suite

[![Language](https://img.shields.io/badge/Language-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20POSIX-orange.svg)](https://www.kernel.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

An academic, interactive software simulator in Standard C modeling **Process Synchronization** using Dijkstra counting semaphores (`empty`, `full`) and a binary `mutex` lock across a circular bounded queue.

Developed as part of the **Self-Learning Micro-Project (SLA-3)** for the Department of Computer Engineering at **Shah & Anchor Kutchhi Engineering College**, affiliated with the University of Mumbai.

---

## 👥 Contributors
- **Tanmay Agrawal** (Roll No: 02 | PRN: 125BTCM1102)
- **Pruthvi Kalel** (Roll No: 23 | PRN: 125BTCM1123)

**Faculty Mentor:** Prof. Vaishali Kosamkar  
**Academic Year:** 2026–2027

---

## 🚀 Features
- **Circular Bounded Queue:** Circular buffer of capacity $N = 5$ with index pointers `in` and `out`.
- **Condition Synchronization:** Automatic thread suspension when the buffer is full (`empty == 0`) or empty (`full == 0`).
- **Mutual Exclusion:** Binary mutex protection across critical section reads and writes.
- **Invariant Verification:** Real-time state telemetry confirming $empty + full = N = 5$ at every step.
- **Three Verification Regimes:**
  - Interactive menu-driven step execution.
  - Saturated burst production with overflow defense.
  - Cold-start consumer underflow interception with pointer wrap-around.

---

## 🛠️ Compilation & Execution

```bash
# Clone the repository
git clone [https://github.com/TanmayAgrawal-02/Producer-Consumer-OS-Simulator.git](https://github.com/TanmayAgrawal-02/Producer-Consumer-OS-Simulator.git)
cd Producer-Consumer-OS-Simulator

# Compile using GCC with POSIX thread support
gcc -Wall -Wextra -pthread main.c -o producer_consumer_sim

# Run the interactive simulator
./producer_consumer_sim