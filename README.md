# Low-Latency Order Book (C++)

A high-performance C++ order book implementation designed to explore and demonstrate **low-latency system design techniques** commonly used in trading systems.

---

## Overview

This project is not just a functional order book—it is a **progressive performance engineering exercise**.

Each version of the system incrementally introduces:

* New functionality
* Architectural improvements
* Low-latency optimizations

The goal is to **measure, compare, and understand** how different design decisions impact:

* Latency (ns / μs)
* Throughput (orders/sec)
* Cache efficiency
* Memory usage

---

## Branching Strategy

Each branch represents a **self-contained evolution** of the system that builds upon previous ideas while introducing measurable improvements:

| Branch | Description                                                |
| ------ | ---------------------------------------------------------- |
| `v1`   | Baseline implementation (limit orders only, simple design) |
| `v2`   | (WIP) Market orders, cancel, and bulk load                 |
| `...`  | Continued experimentation                                  |

---

## Benchmarking Philosophy

Every version is evaluated using a **consistent benchmarking framework**:

* Synthetic order flow generation
* Deterministic workloads (for reproducibility)
* Latency distribution tracking (p50, p99, p99.9)
* Throughput measurement under load

### Key Metrics

* **Latency**

  * Median (p50)
  * Tail latency (p99, p99.9)
* **Throughput**

  * Orders processed per second
* **Stability**

  * Variance under load

---

## Build Instructions

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cd build
make
```

---

## Running

### Run the application

```bash
./order_book_app
```

### Run benchmarks

```bash
./benchmark_app
```

### Run unit tests

```bash
ctest --output-on-failure
```

---

## Performance Focus Areas

This project will explore techniques such as:

* Data structure selection (e.g., maps vs flat structures)
* Memory layout & cache locality
* Avoiding dynamic allocation in hot paths
* Branch prediction optimization
* Lock-free / low-lock designs
* Efficient matching algorithms

---

## Comparing Versions

Each branch includes benchmark results showing:

* Latency improvements
* Throughput gains
* Tradeoffs (e.g., complexity vs performance)
