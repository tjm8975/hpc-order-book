#include "tcOrderBook.hpp"

tsOrder* tcOrderBook::createOrder(uint64_t anId, uint32_t anQuantity, uint32_t anPriceInTicks, bool abIsBuy)
{
    auto lpsOrder = std::make_unique<tsOrder>();
    lpsOrder->mnId = anId;
    lpsOrder->mnQuantity = anQuantity;
    lpsOrder->mnRemaining = anQuantity;
    lpsOrder->mnPriceInTicks = anPriceInTicks;
    lpsOrder->mbIsBuy = abIsBuy;

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

void tcOrderBook::removeOrder(uint64_t anId)
{
    auto lcIter = mcOrders.find(anId);
    if (lcIter != mcOrders.end())
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

        mcOrders.erase(lcIter);  // Deletes object since it's a unique_ptr
    }
    else
    {
        #ifdef DEBUG
        std::cout << "Order ID " << anId << " not found for removal." << std::endl;
        #endif
    }
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

void tcOrderBook::printOrderBook(void) const
{
    #ifdef DEBUG
    std::cout << "Order Book:" << std::endl;

    std::cout << "Bids:" << std::endl;
    for (const auto& [price, level] : mcBids)
    {
        std::cout << "Price: " << price << ", Total Quantity: " << level.getTotalQuantity() << std::endl;
    }

    std::cout << "Asks:" << std::endl;
    for (const auto& [price, level] : mcAsks)
    {
        std::cout << "Price: " << price << ", Total Quantity: " << level.getTotalQuantity() << std::endl;
    }
    #endif
}