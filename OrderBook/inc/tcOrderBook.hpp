#ifndef TCORDERBOOK_HPP
#define TCORDERBOOK_HPP

#include "tsOrder.hpp"
#include "tcPriceLevel.hpp"
#include "constants.hpp"

#include <unordered_map>
#include <map>
#include <iostream>

enum class Side { Bid, Ask };

class tcOrderBook
{
public:
    tsOrder* createOrder(
        uint64_t anId,
        uint32_t anQuantity,
        uint32_t anPriceInTicks,
        bool abIsBuy,
        teOrderType aeOrderType = teOrderType::eeLimit,
        teExecType aeExecType = teExecType::eeGoodTilCanceled);

    void addOrder(tsOrder* apsOrder);

    void removeOrder(uint64_t anId, bool abRemoveFromPriceLevel = true);

    bool cancelOrder(uint64_t anId);

    tcPriceLevel* getBestAsk(void);

    tcPriceLevel* getBestBid(void);

    bool canFullyFill(tsOrder* apsOrder);

    void reset(void);

    void printOrders(void) const;

    void printOrderBook(uint32_t anDepth = Constants::DEFAULT_PRINT_DEPTH) const;

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

    template<Side S>
    bool canFullyFillFromSide(tsOrder* apsOrder)
    {
        uint32_t lnQuantityToFill = apsOrder->mnRemaining;
        auto & lrcSide = GetSide<S>();

        for (const auto& [lnPrice, lrcLevel] : lrcSide)
        {
            if (apsOrder->meOrderType == teOrderType::eeLimit &&
                ((apsOrder->mbIsBuy && apsOrder->mnPriceInTicks < lnPrice) ||
                 (!apsOrder->mbIsBuy && apsOrder->mnPriceInTicks > lnPrice)))
            {
                break; // Best opposite order is not at a price that can be matched
            }

            lnQuantityToFill -= std::min(lnQuantityToFill, lrcLevel.getTotalQuantity());
            if (lnQuantityToFill == 0)
            {
                return true; // Found enough quantity to fill the order
            }
        }

        return false; // Not enough quantity available to fill the order
    }

    // Grant access to private members for unit testing
    friend class tcOrderBookUT;
};

#endif // TCORDERBOOK_HPP