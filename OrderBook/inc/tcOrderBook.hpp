#ifndef TCORDERBOOK_HPP
#define TCORDERBOOK_HPP

#include "tsOrder.hpp"
#include "tcPriceLevel.hpp"

#include <iostream>
#include <unordered_map>
#include <map>

enum class Side { Bid, Ask };

class tcOrderBook
{
public:
    tsOrder* createOrder(uint64_t anId, uint32_t anQuantity, uint32_t anPriceInTicks, bool abIsBuy);

    void addOrder(tsOrder* apsOrder);

    void removeOrder(uint64_t anId, bool abRemoveFromPriceLevel = true);

    bool cancelOrder(uint64_t anId);

    tcPriceLevel* getBestAsk(void);

    tcPriceLevel* getBestBid(void);

    const auto& getBids(void) const
    {
        return mcBids;
    }

    const auto& getAsks(void) const
    {
        return mcAsks;
    }

private:
    std::unordered_map<uint64_t, std::unique_ptr<tsOrder>> mcOrders;

    std::map<uint32_t, tcPriceLevel, std::greater<>> mcBids;
    std::map<uint32_t, tcPriceLevel, std::less<>> mcAsks;

    template<Side S>
    auto& GetSide(void)
    {
        if constexpr (S == Side::Bid)
        {
            return mcBids;
        }
        else
        {
            return mcAsks;
        }
    }

    template<Side S>
    void addToSide(tsOrder* apsOrder)
    {
        auto & lrcSide = GetSide<S>();
        lrcSide[apsOrder->mnPriceInTicks].add(apsOrder);
    }

    template<Side S>
    void removeFromSide(tsOrder* apsOrder)
    {
        auto & lrcSide = GetSide<S>();
        auto lcIter = lrcSide.find(apsOrder->mnPriceInTicks);
        if (lcIter != lrcSide.end())
        {
            lcIter->second.remove(apsOrder);
            if (lcIter->second.isEmpty())
            {
                lrcSide.erase(lcIter);
            }
        }
    }

    // Grant access to private members for unit testing
    friend class tcOrderBookUT;
};

#endif // TCORDERBOOK_HPP