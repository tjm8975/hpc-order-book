#ifndef TCBENCHMARKRUNNER_HPP
#define TCBENCHMARKRUNNER_HPP

#include "tcOrderBook.hpp"
#include "tcMatchingEngine.hpp"
#include "tsMetrics.hpp"
#include "tcOrderIntake.hpp"

#include <cstdint>

class tcBenchmarkRunner
{
public:
    tcBenchmarkRunner(tcOrderBook& arcOrderBook, tcMatchingEngine& arcMatchingEngine);

    tsMetrics runThroughput(uint64_t anNumOrders);

    tsMetrics runPercentile(uint64_t anNumOrders);

private:
    uint64_t mnNumOrdersProcessed = 0;
    tcOrderBook& mrcOrderBook;
    tcMatchingEngine& mrcMatchingEngine;
    tcOrderIntake mcOrderIntake;
};

#endif // TCBENCHMARKRUNNER_HPP