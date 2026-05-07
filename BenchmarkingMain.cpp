#include "tcBenchmarkRunner.hpp"
#include "tsMetrics.hpp"
#include "tcOrderGenerator.hpp"

#include <vector>
#include <iostream>

int main()
{
    tcOrderBook lcOrderBook;
    tcMatchingEngine lcMatchingEngine(lcOrderBook);

    tcBenchmarkRunner lcRunner(lcOrderBook, lcMatchingEngine);

    std::vector<uint64_t> lcTestSizes = {
        10'000,
        100'000,
        1'000'000,
        10'000'000,
    };

    // Priming call
    lcRunner.runThroughput(10'000);
    lcRunner.runPercentile(10'000);
    tsMetrics lsMetrics;

    for (uint64_t lnSize : lcTestSizes)
    {
        lsMetrics = lcRunner.runThroughput(lnSize);
        lsMetrics.print();
        lcOrderBook.reset();

        lsMetrics = lcRunner.runPercentile(lnSize);
        lsMetrics.print();
        lcOrderBook.reset();
    }

    return 0;
}