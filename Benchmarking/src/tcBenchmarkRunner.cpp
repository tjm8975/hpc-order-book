#include "tcBenchmarkRunner.hpp"

#include "tcOrderGenerator.hpp"

#include <chrono>

tcBenchmarkRunner::tcBenchmarkRunner(tcOrderBook& arcOrderBook, tcMatchingEngine& arcMatchingEngine) :
    mrcOrderBook(arcOrderBook),
    mrcMatchingEngine(arcMatchingEngine),
    mcOrderIntake(arcOrderBook, arcMatchingEngine)
{
}

tsMetrics tcBenchmarkRunner::run(uint64_t anNumOrders)
{
    tcOrderGenerator lsOrderGen;
    tsMetrics lsMetrics;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (uint64_t i = 0; i < anNumOrders; ++i) {
        auto order = lsOrderGen.generateOrder(mnNumOrdersProcessed++);
        mcOrderIntake.submitOrder(order.mnId, order.mnQuantity, order.mrPrice, order.mbIsBuy);
        lsMetrics.lnTotalOrders++;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    lsMetrics.lrTotalTimeSec = std::chrono::duration<double>(endTime - startTime).count();

    return lsMetrics;
}