# Order Book – v2 (WIP)

## Overview

This branch provides builds upon the baseline implementation from `v1` by introducing a new order type.

The focus of this iteration is to:

* Handle market orders
* Handle order cancelling
* Allow bulk loading of commands when using the CLI
  * See input/example.txt for an example file

---

## Performance Metrics (10 million orders)

### Latency (nanoseconds)

| Metric | v1   | v2   |
| ------ | ---- | ---- |
| p50    | 110  | XXXX |
| p90    | 280  | XXXX |
| p99    | 505  | XXXX |
| p99.9  | 1569 | XXXX |

---

### Throughput (millions)

| Metric     | v1    | v2    |
| ---------- | ----- | ----- |
| Orders/sec | 6.624 | X.XXX |
