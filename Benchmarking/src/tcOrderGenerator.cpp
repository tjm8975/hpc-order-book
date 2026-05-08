#include "tcOrderGenerator.hpp"

tcOrderGenerator::tcOrderGenerator(uint32_t seed) :
    mcRng(seed),
    mcQuantityDist(1, 100),   // Quantity between 1 and 100
    mcPriceDist(100.0, 0.5),  // Average price of 100 with std dev of 0.5, normal distribution
    mcSideDist(0, 1),         // Buy or Sell
    mcOrderTypeDist(0, static_cast<int>(teOrderType::eeLast) - 1), // Order type
    mcLimitExecTypeDist(0, static_cast<int>(teExecType::eeLast) - 1), // Exec types for Limit orders
    mcMarketExecTypeDist(1, static_cast<int>(teExecType::eeLast) - 1) // Exec types for Market orders
{
}

tcOrderGenerator::tsGenOrder tcOrderGenerator::generateOriginalOrder(uint64_t anId)
{
    return tsGenOrder{
        anId,
        mcQuantityDist(mcRng),
        mcPriceDist(mcRng),
        mcSideDist(mcRng) == 1, // true for buy, false for sell
        teOrderType::eeLimit,
        teExecType::eeGoodTilCanceled
    };
}

tcOrderGenerator::tsGenOrder tcOrderGenerator::generateOrder(uint64_t anId)
{
    teOrderType leOrderType = static_cast<teOrderType>(mcOrderTypeDist(mcRng));
    teExecType leExecType = teExecType::eeGoodTilCanceled;
    if (leOrderType == teOrderType::eeLimit)
    {
        leExecType = static_cast<teExecType>(mcLimitExecTypeDist(mcRng));
    }
    else
    {
        leExecType = static_cast<teExecType>(mcMarketExecTypeDist(mcRng));
    }

    return tsGenOrder{
        anId,
        mcQuantityDist(mcRng),
        mcPriceDist(mcRng),
        mcSideDist(mcRng) == 1, // true for buy, false for sell
        leOrderType,
        leExecType
    };
}