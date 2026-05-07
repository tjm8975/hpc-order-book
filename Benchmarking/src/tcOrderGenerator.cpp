#include "tcOrderGenerator.hpp"

tcOrderGenerator::tcOrderGenerator(uint32_t seed) :
    mcRng(seed),
    mcQuantityDist(1, 100),  // Quantity between 1 and 100
    mcPriceDist(100.0, 0.5), // Average price of 100 with std dev of 0.5, normal distribution
    mcSideDist(0, 1)
{
}

tcOrderGenerator::GenOrder tcOrderGenerator::generateOrder(uint64_t anId)
{
    return GenOrder{
        anId,
        mcQuantityDist(mcRng),
        mcPriceDist(mcRng),
        mcSideDist(mcRng) == 1 // true for buy, false for sell
    };
}