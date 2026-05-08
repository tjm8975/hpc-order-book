#include "tcBenchmarkRunner.hpp"

#include "tcOrderGenerator.hpp"

#include <chrono>
#include <thread>

tcBenchmarkRunner::tcBenchmarkRunner(tcOrderBook& arcOrderBook, tcMatchingEngine& arcMatchingEngine) :
    mrcOrderBook(arcOrderBook),
    mrcMatchingEngine(arcMatchingEngine),
    mcOrderIntake(arcOrderBook, arcMatchingEngine)
{
}

tsMetrics tcBenchmarkRunner::runOriginalThroughput(uint64_t anNumOrders)
{
    tcOrderGenerator lsOrderGen;
    tsMetrics lsMetrics;

    auto lnStart = std::chrono::high_resolution_clock::now();

    for (uint64_t i = 0; i < anNumOrders; i++)
    {
        auto lsOrder = lsOrderGen.generateOriginalOrder(mnNumOrdersProcessed++);
        mcOrderIntake.submitOrder(
            lsOrder.mnId,
            lsOrder.mnQuantity,
            lsOrder.mrPrice,
            lsOrder.mbIsBuy,
            lsOrder.meOrderType,
            lsOrder.meExecType);
        lsMetrics.mnTotalOrders++;
    }

    auto lnEnd = std::chrono::high_resolution_clock::now();
    lsMetrics.mrTotalTimeSec = std::chrono::duration<double>(lnEnd - lnStart).count();

    return lsMetrics;
}

tsMetrics tcBenchmarkRunner::runThroughput(uint64_t anNumOrders)
{
    tcOrderGenerator lsOrderGen;
    tsMetrics lsMetrics;

    auto lnStart = std::chrono::high_resolution_clock::now();

    for (uint64_t i = 0; i < anNumOrders; i++)
    {
        auto lsOrder = lsOrderGen.generateOrder(mnNumOrdersProcessed++);
        mcOrderIntake.submitOrder(
            lsOrder.mnId,
            lsOrder.mnQuantity,
            lsOrder.mrPrice,
            lsOrder.mbIsBuy,
            lsOrder.meOrderType,
            lsOrder.meExecType);
        lsMetrics.mnTotalOrders++;
    }

    auto lnEnd = std::chrono::high_resolution_clock::now();
    lsMetrics.mrTotalTimeSec = std::chrono::duration<double>(lnEnd - lnStart).count();

    return lsMetrics;
}

tsMetrics tcBenchmarkRunner::runOriginalPercentile(uint64_t anNumOrders)
{
    tcOrderGenerator lsOrderGen;
    tsMetrics lsMetrics;
    const int lnSampleRate = 10; // Sample every 10th order for latency measurement

    lsMetrics.mcLatenciesNs.reserve(anNumOrders / lnSampleRate); // reserve space for sampled latencies

    for (uint64_t i = 0; i < anNumOrders; i++)
    {
        auto lsOrder = lsOrderGen.generateOriginalOrder(mnNumOrdersProcessed++);

        // Sample latency for every 10th order to avoid overhead of timing every single order
        if (i % lnSampleRate == 0)
        {
            auto lnT1 = std::chrono::high_resolution_clock::now();
            mcOrderIntake.submitOrder(
                lsOrder.mnId,
                lsOrder.mnQuantity,
                lsOrder.mrPrice,
                lsOrder.mbIsBuy,
                lsOrder.meOrderType,
                lsOrder.meExecType);
            auto lnT2 = std::chrono::high_resolution_clock::now();

            double lrNs = std::chrono::duration_cast<std::chrono::nanoseconds>(lnT2 - lnT1).count();
            lsMetrics.mcLatenciesNs.push_back(lrNs);
        }
        else
        {
            mcOrderIntake.submitOrder(
                lsOrder.mnId,
                lsOrder.mnQuantity,
                lsOrder.mrPrice,
                lsOrder.mbIsBuy,
                lsOrder.meOrderType,
                lsOrder.meExecType);
        }

        lsMetrics.mnTotalOrders++;
    }

    return lsMetrics;
}

tsMetrics tcBenchmarkRunner::runPercentile(uint64_t anNumOrders)
{
    tcOrderGenerator lsOrderGen;
    tsMetrics lsMetrics;
    const int lnSampleRate = 10; // Sample every 10th order for latency measurement

    lsMetrics.mcLatenciesNs.reserve(anNumOrders / lnSampleRate); // reserve space for sampled latencies

    for (uint64_t i = 0; i < anNumOrders; i++)
    {
        auto lsOrder = lsOrderGen.generateOrder(mnNumOrdersProcessed++);

        // Sample latency for every 10th order to avoid overhead of timing every single order
        if (i % lnSampleRate == 0)
        {
            auto lnT1 = std::chrono::high_resolution_clock::now();
            mcOrderIntake.submitOrder(
                lsOrder.mnId,
                lsOrder.mnQuantity,
                lsOrder.mrPrice,
                lsOrder.mbIsBuy,
                lsOrder.meOrderType,
                lsOrder.meExecType);
            auto lnT2 = std::chrono::high_resolution_clock::now();

            double lrNs = std::chrono::duration_cast<std::chrono::nanoseconds>(lnT2 - lnT1).count();
            lsMetrics.mcLatenciesNs.push_back(lrNs);
        }
        else
        {
            mcOrderIntake.submitOrder(
                lsOrder.mnId,
                lsOrder.mnQuantity,
                lsOrder.mrPrice,
                lsOrder.mbIsBuy,
                lsOrder.meOrderType,
                lsOrder.meExecType);
        }

        lsMetrics.mnTotalOrders++;
    }

    return lsMetrics;
}