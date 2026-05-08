# Order Book – v3

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

| Metric | v1  | v2  | v3  |
| ------ | --- | --- | --- |
| p50    | 71  | 72  | 73  |
| p90    | 141 | 143 | 146 |
| p99    | 254 | 261 | 264 |
| p99.9  | 558 | 690 | 577 |

---

### GTC Limit Order Throughput (millions)

| Metric     | v1    | v2    | v3    |
| ---------- | ----- | ----- | ----- |
| Orders/sec | 7.005 | 6.534 | 6.811 |

---

### All Orders Latency (nanoseconds)

| Metric | v2  | v3  |
| ------ | --- | --- |
| p50    | 57  | 52  |
| p90    | 108 | 86  |
| p99    | 186 | 143 |
| p99.9  | 283 | 230 |

---

### All Orders Throughput (millions)

| Metric     | v2     | v3     |
| ---------- | ------ | ------ |
| Orders/sec | 13.613 | 14.954 |

---

## Takeaways

The GTC Limit order throughput and latency is roughly the same as v2. This is expected, since only a single conditional check was added to the matching engine to skip GTC Market orders. As the run progresses, the check likely has no impact as the branch prediction no longer expects that branch to be executed since the benchmark is not passing any invalid GTC Market orders.

The throughput and latency for All Orders is better than in v2, but this is most likely due to the fact that Limit orders can now be IOC, resulting in a shorter book depth.

I am also noticing inconsistencies between runs since I am just running these benchmarks on my desktop. Averaging across 5 runs helps, but there are still some inaccuracies so the metrics should not be taken as exact.
