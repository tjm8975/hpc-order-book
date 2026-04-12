# Order Book – v1

## Overview

This branch provides a baseline implementation of a single-symbol order book with limit orders only.

---

## Core Functionality

The order book supports:

* Limit order insertion (bid/ask)
* Price-time priority matching
* Order matching and trade execution
* Basic order lifecycle management

### Main Components

* **Order** – Represents an individual order
* **PriceLevel** – Aggregates orders at a given price
* **OrderBook** – Maintains bid/ask sides
* **MatchingEngine** – Handles matching logic
* **OrderIntake** – Entry point for incoming orders

---

## Performance Metrics (10 million orders)

### Latency (nanoseconds)

| Metric | v1   |
| ------ | ---- |
| p50    | 110  |
| p90    | 280  |
| p99    | 505  |
| p99.9  | 1569 |

---

### Throughput (millions)

| Metric     | v1    |
| ---------- | ----- |
| Orders/sec | 6.624 |
