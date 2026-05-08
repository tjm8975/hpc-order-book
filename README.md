# Order Book – v4

## Overview

This branch provides builds upon the baseline implementation from `v3` by introducing a new order type.

The focus of this iteration is to:

* Add FOK support for Limit and Market orders
* Add reset functionality to the CLI

---

## Performance Metrics (10 million orders, average across 5 runs)

### GTC Limit Order Latency (nanoseconds)

| Metric | v1  | v2  | v3  | v4  |
| ------ | --- | --- | --- | --- |
| p50    | 71  | 72  | 73  | 73  |
| p90    | 141 | 143 | 146 | 145 |
| p99    | 254 | 261 | 264 | 262 |
| p99.9  | 558 | 690 | 577 | 568 |

---

### GTC Limit Order Throughput (millions)

| Metric     | v1    | v2    | v3    | v4    |
| ---------- | ----- | ----- | ----- | ----- |
| Orders/sec | 7.005 | 6.534 | 6.811 | 6.365 |

---

### All Orders Latency (nanoseconds)

| Metric | v2  | v3  | v4  |
| ------ | --- | --- | --- |
| p50    | 57  | 52  | 54  |
| p90    | 108 | 86  | 82  |
| p99    | 186 | 143 | 135 |
| p99.9  | 283 | 230 | 216 |

---

### All Orders Throughput (millions)

| Metric     | v2     | v3     | v4     |
| ---------- | ------ | ------ | ------ |
| Orders/sec | 13.613 | 14.954 | 14.867 |

---

## Takeaways

As expected with adding more checking in matching engine, the GTC Limit order metrics are slightly worse than previous versions. When running with all order types, the metrics are better since FOK orders have a chance of cancelling early and not actually doing the trade execution logic.

This represents the complete functionality of the order book and future versions will focus on optimizations to improve latencies and throughput without adding new functionality.
