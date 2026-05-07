#ifndef TCORDERGENERATOR_HPP
#define TCORDERGENERATOR_HPP

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
    };

    GenOrder generateOrder(uint64_t anId);

private:
    std::mt19937 mcRng;
    std::uniform_int_distribution<uint32_t> mcQuantityDist;
    std::normal_distribution<double> mcPriceDist;
    std::uniform_int_distribution<int> mcSideDist;
};

#endif // TCORDERGENERATOR_HPP