#include "tcMatchingEngine.hpp"
#include "constants.hpp"

#include <iostream>
#include <algorithm>

tcMatchingEngine::tcMatchingEngine(tcOrderBook& arcOrderBook) :
    mrcOrderBook(arcOrderBook)
{
}

void tcMatchingEngine::process(tsOrder* apsIncomingOrder)
{
    tcMatchingEngine::match(apsIncomingOrder);

    if (apsIncomingOrder->mnRemaining > 0)
    {
        mrcOrderBook.addOrder(apsIncomingOrder);
    }
    else
    {
        // Remove from book's order map (delete order object) without attempting
        // to remove from price level since this order was completely filled and
        // was never added to the price level in the first place
        mrcOrderBook.removeOrder(apsIncomingOrder->mnId, false);
    }
}

void tcMatchingEngine::match(tsOrder* apsIncomingOrder)
{
    while (apsIncomingOrder->mnRemaining > 0)
    {
        tcPriceLevel* lpcBestOppositeLevel = apsIncomingOrder->mbIsBuy ? mrcOrderBook.getBestAsk() : mrcOrderBook.getBestBid();

        if (lpcBestOppositeLevel == nullptr)
        {
            break; // No opposite orders to match against
        }

        tsOrder* lpsRestingOrder = lpcBestOppositeLevel->getBestOrder();

        if ((apsIncomingOrder->mbIsBuy && apsIncomingOrder->mnPriceInTicks < lpsRestingOrder->mnPriceInTicks) ||
            (!apsIncomingOrder->mbIsBuy && apsIncomingOrder->mnPriceInTicks > lpsRestingOrder->mnPriceInTicks))
        {
            break; // Best opposite order is not at a price that can be matched
        }

        uint32_t lnTradeQuantity = std::min(apsIncomingOrder->mnRemaining, lpsRestingOrder->mnRemaining);
        tcMatchingEngine::executeTrade(apsIncomingOrder, lpsRestingOrder, lnTradeQuantity, lpcBestOppositeLevel);

        if (lpsRestingOrder->mnRemaining == 0)
        {
            mrcOrderBook.removeOrder(lpsRestingOrder->mnId);
        }
    }
}

void tcMatchingEngine::executeTrade(tsOrder* apsTaker, tsOrder* apsMaker, uint32_t anQuantity, tcPriceLevel* apcPriceLevel)
{
    apsTaker->mnRemaining -= anQuantity;
    apsMaker->mnRemaining -= anQuantity;
    apcPriceLevel->executeTrade(anQuantity);

    #ifdef DEBUG
    std::cout << "Trade: " << anQuantity
              << " @ " << apsMaker->mnPriceInTicks * Constants::DOLLARS_PER_TICK
              << " (taker " << apsTaker->mnId
              << " vs maker " << apsMaker->mnId << ")"
              << std::endl;
    #endif
}