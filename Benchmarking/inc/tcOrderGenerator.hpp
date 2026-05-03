#ifndef TCORDERGENERATOR_HPP
#define TCORDERGENERATOR_HPP

#include "tsOrder.hpp"

#include <random>
#include <cstdint>

class tcOrderGenerator
{
public:
    tcOrderGenerator(uint32_t seed = 42);

    struct GenOrder
    {
        uint64_t mnId;
        uint32_t mnQuantity;
        double mrPrice;
        bool mbIsBuy;
        teOrderType meOrderType;
    };

    GenOrder generateOrder(uint64_t anId);

private:
    std::mt19937 mcRng;
    std::uniform_int_distribution<uint32_t> mcQuantityDist;
    std::uniform_real_distribution<double> mcPriceDist;
    std::uniform_int_distribution<int> mcSideDist;
    std::uniform_int_distribution<int> mcTypeDist;
};

#endif // TCORDERGENERATOR_HPP