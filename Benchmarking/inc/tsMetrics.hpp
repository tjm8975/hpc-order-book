#ifndef TSMETRICS_HPP
#define TSMETRICS_HPP

#include <cstdint>
#include <iostream>

struct tsMetrics
{
    uint64_t lnTotalOrders = 0;
    uint64_t lnTotalTrades = 0;

    double lrTotalTimeSec = 0.0;

    void print() const {
        std::cout << "\n==== Benchmark Results ====\n";
        std::cout << "Total Orders: " << lnTotalOrders << "\n";
        std::cout << "Total Time (s): " << lrTotalTimeSec << "\n";
        std::cout << "Throughput (orders/sec): "
                  << (lnTotalOrders / lrTotalTimeSec) << "\n";
    }
};

#endif // TSMETRICS_HPP