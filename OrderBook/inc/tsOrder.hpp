#ifndef TSORDER_HPP
#define TSORDER_HPP

#include <cstdint>
#include <memory>

enum class teType
{
    eeLimit,
    eeMarket,
    eeLast  // Used to find total number of order types (eeLast - 1)
};

static const std::string TypeStrings[] = {
    "Limit",
    "Market"
};

static std::string typeToString(teType aeType)
{
    return TypeStrings[static_cast<int>(aeType)];
};

struct tsOrder
{
    uint64_t mnId;
    uint32_t mnQuantity;
    uint32_t mnPriceInTicks;
    uint32_t mnRemaining;
    bool mbIsBuy;
    teType meType;

    tsOrder* mpsPrev = nullptr;
    tsOrder* mpsNext = nullptr;
};

#endif // TSORDER_HPP