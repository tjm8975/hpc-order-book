#ifndef TSMETRICS_HPP
#define TSMETRICS_HPP

#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>

struct tsMetrics
{
    uint64_t mnTotalOrders = 0;
    double mrTotalTimeSec = 0.0;

    std::vector<uint64_t> mcLatenciesNs; // Latencies in nanoseconds

    static uint64_t percentile(std::vector<uint64_t> & arcData, double arPercentile)
    {
        size_t lnIdx = static_cast<size_t>(arPercentile * arcData.size());
        if (lnIdx >= arcData.size())
        {
            lnIdx = arcData.size() - 1;
        }

        std::nth_element(arcData.begin(), arcData.begin() + lnIdx, arcData.end());
        return arcData[lnIdx];
    }

    void print() const
    {
        if (mcLatenciesNs.empty())
        {
            std::cout << "\n==== Throughput Results ====\n";
            std::cout << "Total Orders: " << mnTotalOrders << "\n";
            std::cout << "Total Time (s): " << mrTotalTimeSec << "\n";
            std::cout << "Throughput (orders/sec): "
                    << (mnTotalOrders / mrTotalTimeSec) << "\n";
        }
        else
        {
            auto lcData = mcLatenciesNs; // copy (nth_element mutates)

            uint64_t ln50p  = percentile(lcData, 0.50);
            uint64_t ln90p  = percentile(lcData, 0.90);
            uint64_t ln99p  = percentile(lcData, 0.99);
            uint64_t ln999p = percentile(lcData, 0.999);

            std::cout << "\n==== Latency Results ====\n";
            std::cout << "p50  = " << ln50p  << " ns)\n";
            std::cout << "p90  = " << ln90p  << " ns)\n";
            std::cout << "p99  = " << ln99p  << " ns)\n";
            std::cout << "p99.9= " << ln999p << " ns)\n\n\n";
        }
    }
};

#endif // TSMETRICS_HPP