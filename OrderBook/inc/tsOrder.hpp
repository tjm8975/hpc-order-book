#ifndef TSORDER_HPP
#define TSORDER_HPP

#include <cstdint>
#include <memory>

enum class teOrderType
{
    eeLimit,
    eeMarket,
    eeLast  // Used to find total number of order types (eeLast - 1)
};

enum class teExecType
{
    eeGoodTilCanceled,
    eeImmediateOrCancel,
    eeFillOrKill,
    eeLast  // Used to find total number of order types (eeLast - 1)
};

static const std::string sacOrderTypeStrings[] = {
    "Limit",
    "Market"
};

static const std::string sacExecTypeStrings[] = {
    "GTC",
    "IOC",
    "FOK"
};

static std::string orderTypeToString(teOrderType aeOrderType)
{
    return sacOrderTypeStrings[static_cast<int>(aeOrderType)];
};

static std::string execTypeToString(teExecType aeExecType)
{
    return sacExecTypeStrings[static_cast<int>(aeExecType)];
};

struct tsOrder
{
    uint64_t mnId;
    uint32_t mnQuantity;
    uint32_t mnPriceInTicks;
    uint32_t mnRemaining;
    bool mbIsBuy;
    teOrderType meOrderType;
    teExecType meExecType;

    tsOrder* mpsPrev = nullptr;
    tsOrder* mpsNext = nullptr;
};

#endif // TSORDER_HPP