#include "tcPriceLevel.hpp"

#include <iomanip>
#include <iostream>

void tcPriceLevel::add(tsOrder* apsOrder)
{
    apsOrder->mpsPrev = mpsTail;
    apsOrder->mpsNext = nullptr;

    if (mpsTail != nullptr)
    {
        mpsTail->mpsNext = apsOrder;
    }
    else
    {
        mpsHead = apsOrder;
    }

    mpsTail = apsOrder;
    mnTotalQuantity += apsOrder->mnRemaining;
}

void tcPriceLevel::remove(tsOrder* apsOrder)
{
    // It is expected that caller will only attempt to remove orders that are
    // actually on this price level, so no need to verify that here
    
    if (apsOrder->mpsPrev != nullptr)
    {
        apsOrder->mpsPrev->mpsNext = apsOrder->mpsNext;
    }
    else
    {
        mpsHead = apsOrder->mpsNext;
    }

    if (apsOrder->mpsNext != nullptr)
    {
        apsOrder->mpsNext->mpsPrev = apsOrder->mpsPrev;
    }
    else
    {
        mpsTail = apsOrder->mpsPrev;
    }

    apsOrder->mpsPrev = nullptr;
    apsOrder->mpsNext = nullptr;
    mnTotalQuantity -= apsOrder->mnRemaining;
}

void tcPriceLevel::executeTrade(uint32_t anQuantity)
{
    mnTotalQuantity -= anQuantity;
}

// =============================================================================
void tcPriceLevel::printLevel(double arPrice) const
{
    std::cout << std::setw(10) << mnTotalQuantity << " @ $" <<
        std::fixed << std::setprecision(2) << arPrice << std::endl;
}