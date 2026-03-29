#ifndef TCPRICELEVEL_HPP
#define TCPRICELEVEL_HPP

#include "tsOrder.hpp"

class tcPriceLevel
{
public:
    void add(tsOrder* apsOrder);

    void remove(tsOrder* apsOrder);

    tsOrder* getBestOrder(void) const
    {
        return mpsHead;
    };

    bool isEmpty(void) const
    {
        return mpsHead == nullptr;
    };

    uint32_t getTotalQuantity(void) const
    {
        return mnTotalQuantity;
    };

    void executeTrade(uint32_t anQuantity);

private:
    tsOrder* mpsHead = nullptr;
    tsOrder* mpsTail = nullptr;
    uint32_t mnTotalQuantity = 0;
};

#endif // TCPRICELEVEL_HPP