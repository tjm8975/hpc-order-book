#ifndef TSORDER_HPP
#define TSORDER_HPP

#include <cstdint>
#include <memory>

struct tsOrder
{
    uint64_t mnId;
    uint32_t mnQuantity;
    uint32_t mnPriceInTicks;
    uint32_t mnRemaining;
    bool mbIsBuy;

    tsOrder* mpsPrev = nullptr;
    tsOrder* mpsNext = nullptr;
};

#endif // TSORDER_HPP