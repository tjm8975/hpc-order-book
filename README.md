# Order Book – v3 (WIP)

## Overview

This branch provides builds upon the baseline implementation from `v2` by introducing a new order type.

The focus of this iteration is to:

* Add IOC support for Limit orders
  * Market orders are already treated as IOC orders. GTC is not supported for Market orders at this time.

It was also discovered that some improvements could be made to the benchmarking such as:

* Resetting the order book in between runs
  * Having more orders in the book results in decreased performance
  * Want to avoid previous tests impacting the results of other tests
* Running the orignal benchmark each time
  * With the addition of market orders, which strictly remove orders from the book, performance is better on average since there are less orders in the book
  * Having a benchmark that only submits GTC Limit orders allows for better comparisons between iterations
* Using normal distribution instead of uniform distribution for the price of each order
  * More accurately represents price action in a high-volume market

---

## Performance Metrics (10 million orders, average across 5 runs)

### GTC Limit Order Latency (nanoseconds)

| Metric | v1   | v2   | v3   |
| ------ | ---- | ---- | ---- |
| p50    | 110  | 112  | 71   |
| p90    | 280  | 289  | 141  |
| p99    | 505  | 530  | 254  |
| p99.9  | 1569 | 1640 | 538  |

---

### GTC Limit Order Throughput (millions)

| Metric     | v1    | v2    | v3    |
| ---------- | ----- | ----- | ----- |
| Orders/sec | 6.624 | 6.230 | 6.650 |

---

### All Orders Latency (nanoseconds)

| Metric | v1   | v2   | v3   |
| ------ | ---- | ---- | ---- |
| p50    | 110  | 112  | 82   |
| p90    | 280  | 289  | 173  |
| p99    | 505  | 530  | 304  |
| p99.9  | 1569 | 1640 | 698  |

---

### All Orders Throughput (millions)

| Metric     | v1    | v2    | v3     |
| ---------- | ----- | ----- | ------ |
| Orders/sec | 6.624 | 6.230 | 15.249 |

---

## Takeaways
