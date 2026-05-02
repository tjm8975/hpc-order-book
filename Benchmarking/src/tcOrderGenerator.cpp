#include "tcOrderGenerator.hpp"

tcOrderGenerator::tcOrderGenerator(uint32_t seed) :
    mcRng(seed),
    mcQuantityDist(1, 100),   // Quantity between 1 and 100
    mcPriceDist(95.0, 105.0), // Price between 95.0 and 105.0
    mcSideDist(0, 1),         // Buy or Sell
    mcTypeDist(0, static_cast<int>(teType::eeLast) - 1)
{
}

tcOrderGenerator::GenOrder tcOrderGenerator::generateOrder(uint64_t anId)
{
    return GenOrder{
        anId,
        mcQuantityDist(mcRng),
        mcPriceDist(mcRng),
        mcSideDist(mcRng) == 1, // true for buy, false for sell
        static_cast<teType>(mcTypeDist(mcRng))
    };
}