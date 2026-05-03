#include "tcOrderBook.hpp"

#include <iomanip>
#include <iostream>
#include <ranges>

tsOrder* tcOrderBook::createOrder(
    uint64_t anId,
    uint32_t anQuantity,
    uint32_t anPriceInTicks,
    bool abIsBuy,
    teOrderType aeOrderType,
    teExecType aeExecType)
{
    auto lpsOrder = std::make_unique<tsOrder>();
    lpsOrder->mnId = anId;
    lpsOrder->mnQuantity = anQuantity;
    lpsOrder->mnRemaining = anQuantity;
    lpsOrder->mnPriceInTicks = anPriceInTicks;
    lpsOrder->mbIsBuy = abIsBuy;
    lpsOrder->meOrderType = aeOrderType;
    lpsOrder->meExecType = aeExecType;

    tsOrder* lpsOrderPtr = lpsOrder.get();
    mcOrders[anId] = std::move(lpsOrder);
    return lpsOrderPtr;
}

void tcOrderBook::addOrder(tsOrder* apsOrder)
{
    if (apsOrder->mbIsBuy)
    {
        addToSide<Side::Bid>(apsOrder);
    }
    else
    {
        addToSide<Side::Ask>(apsOrder);
    }
}

void tcOrderBook::removeOrder(uint64_t anId, bool abRemoveFromPriceLevel)
{
    auto lcIter = mcOrders.find(anId);
    if (lcIter != mcOrders.end())
    {
        if (abRemoveFromPriceLevel)
        {
            tsOrder* lpsOrder = lcIter->second.get();
            if (lpsOrder->mbIsBuy)
            {
                removeFromSide<Side::Bid>(lpsOrder);
            }
            else
            {
                removeFromSide<Side::Ask>(lpsOrder);
            }
        }

        mcOrders.erase(lcIter);  // Deletes object since it's a unique_ptr
    }
    else
    {
        #ifdef DEBUG
        std::cout << "Order ID " << anId << " not found for removal." << std::endl;
        #endif
    }
}

bool tcOrderBook::cancelOrder(uint64_t anId)
{
    auto lcIter = mcOrders.find(anId);
    if (lcIter != mcOrders.end())
    {
        removeOrder(anId);
        return true;
    }

    return false; // Order ID not found
}

tcPriceLevel* tcOrderBook::getBestAsk(void)
{
    if (mcAsks.empty())
    {
        return nullptr;
    }

    return &mcAsks.begin()->second;
}

tcPriceLevel* tcOrderBook::getBestBid(void)
{
    if (mcBids.empty())
    {
        return nullptr;
    }

    return &mcBids.begin()->second;
}

// =============================================================================
void tcOrderBook::printOrders(void) const
{
    if (mcOrders.empty())
    {
        std::cout << "No active orders." << std::endl;
    }
    else
    {
        std::cout << "\nActive Orders:" << std::endl;
        for (const auto& [lnId, lpsOrder] : mcOrders)
        {
            double lrPrice = lpsOrder->mnPriceInTicks * Constants::DOLLARS_PER_TICK;
            std::cout << "ID: " << lnId
                      << " | Side: " << (lpsOrder->mbIsBuy ? "Buy" : "Sell")
                      << " | Qty: " << lpsOrder->mnRemaining
                      << " | Price: $" << std::fixed << std::setprecision(2) << lrPrice
                      << std::endl;
        }
    }
}

void tcOrderBook::printOrderBook(uint32_t anDepth) const
{
    if (mcAsks.empty() && mcBids.empty())
    {
        std::cout << "Order book is empty." << std::endl;
        return;
    }

    std::cout << "\n======= ORDER BOOK =======\n" << std::endl;

    // Need to reverse asks after finding best levels since they are stored in
    // ascending order
    for (const auto& [lnTicks, lrcLevel] : mcAsks | std::views::take(anDepth) | std::views::reverse)
    {
        lrcLevel.printLevel(lnTicks * Constants::DOLLARS_PER_TICK);
    }

    std::cout << "----------" << std::endl;

    for (const auto& [lnTicks, lrcLevel] : mcBids | std::views::take(anDepth))
    {
        lrcLevel.printLevel(lnTicks * Constants::DOLLARS_PER_TICK);
    }

    // Show spread and market stock price (midpoint of best bid and ask)
    uint32_t lnBestAsk = mcAsks.empty() ? 0 : mcAsks.begin()->first;
    uint32_t lnBestBid = mcBids.empty() ? 0 : mcBids.begin()->first;
    if (lnBestAsk > 0 && lnBestBid > 0)
    {
        double lrSpread = (lnBestAsk - lnBestBid) * Constants::DOLLARS_PER_TICK;
        double lrMidpoint = (lnBestAsk + lnBestBid) * 0.5 * Constants::DOLLARS_PER_TICK;
        std::cout << "\nSpread: $" << std::fixed << std::setprecision(2) << lrSpread << std::endl;
        std::cout << "Market: $" << std::fixed << std::setprecision(2) << lrMidpoint << std::endl;
    }

    std::cout << "\n==========================" << std::endl;
}