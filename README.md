# Order Book – v2

## Overview

This branch provides builds upon the baseline implementation from `v1` by introducing a new order type.

The focus of this iteration is to:

* Handle market orders
  * Implemented as Immediate or Cancel (IOC) orders, so no unfilled quantity is added to book
* Handle order cancelling
* Allow bulk loading of commands when using the CLI
  * See input/example.txt for an example file

---

## Performance Metrics (10 million orders, average across 5 runs)

### Limit Order Latency (nanoseconds)

| Metric | v1   | v2   |
| ------ | ---- | ---- |
| p50    | 110  | 72   |
| p90    | 280  | 143  |
| p99    | 505  | 261  |
| p99.9  | 1569 | 690  |

---

### Limit Order Throughput (millions)

| Metric     | v1    | v2    |
| ---------- | ----- | ----- |
| Orders/sec | 6.624 | 6.534 |

---

### All Orders Latency (nanoseconds)

| Metric | v2   |
| ------ | ---- |
| p50    | 57   |
| p90    | 108  |
| p99    | 186  |
| p99.9  | 283  |

---

### All Orders Throughput (millions)

| Metric     | v2     |
| ---------- | ------ |
| Orders/sec | 13.613 |

---

## Takeaways

As expected, the performance of the system is slightly worse now that we have to handle multiple order types.

Although the processing of these order types is very similar and reuse most of the same logic, there is still an extra check necessary when processing the order to determine the type, which explains the performance cost.

More complexity will be added in the next couple of iterations as support for more order types is implemented.
