#include "tcOrderGenerator.hpp"

tcOrderGenerator::tcOrderGenerator(uint32_t seed) :
    mcRng(seed),
    mcQuantityDist(1, 100),   // Quantity between 1 and 100
    mcPriceDist(100.0, 0.5),  // Average price of 100 with std dev of 0.5, normal distribution
    mcSideDist(0, 1),         // Buy or Sell
    mcTypeDist(0, static_cast<int>(teType::eeLast) - 1)
{
}

tcOrderGenerator::tsGenOrder tcOrderGenerator::generateOriginalOrder(uint64_t anId)
{
    return tsGenOrder{
        anId,
        mcQuantityDist(mcRng),
        mcPriceDist(mcRng),
        mcSideDist(mcRng) == 1, // true for buy, false for sell
        teType::eeLimit
    };
}

tcOrderGenerator::tsGenOrder tcOrderGenerator::generateOrder(uint64_t anId)
{
    return tsGenOrder{
        anId,
        mcQuantityDist(mcRng),
        mcPriceDist(mcRng),
        mcSideDist(mcRng) == 1, // true for buy, false for sell
        static_cast<teType>(mcTypeDist(mcRng))
    };
}