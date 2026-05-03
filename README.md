# Order Book – v3

## Overview

This branch provides builds upon the baseline implementation from `v2` by introducing a new order type.

The focus of this iteration is to:

* Add IOC support for Limit orders
  * Market orders are already treated as IOC orders. GTC is not supported for Market orders at this time.

---

## Performance Metrics (10 million orders, average across 5 runs)

### Latency (nanoseconds)

| Metric | v1   | v2   | v3   |
| ------ | ---- | ---- | ---- |
| p50    | 110  | 112  | XXXX |
| p90    | 280  | 289  | XXXX |
| p99    | 505  | 530  | XXXX |
| p99.9  | 1569 | 1640 | XXXX |

---

### Throughput (millions)

| Metric     | v1    | v2    | v3    |
| ---------- | ----- | ----- | ----- |
| Orders/sec | 6.624 | 6.230 | XXXX  |

---

## Takeaways
