#ifndef TCORDERINTAKE_HPP
#define TCORDERINTAKE_HPP

#include "tcOrderBook.hpp"
#include "tcMatchingEngine.hpp"

class tcOrderIntake
{
public:
    tcOrderIntake(
        tcOrderBook& arcOrderBook,
        tcMatchingEngine& arcMatchingEngine);

    void submitOrder(
        uint64_t anId,
        uint32_t anQuantity,
        double arPrice,
        bool abIsBuy,
        teOrderType aeOrderType = teOrderType::eeLimit,
        teExecType aeExecType = teExecType::eeGoodTilCanceled);

    bool cancelOrder(uint64_t anId);

private:
    tcOrderBook& mrcOrderBook;
    tcMatchingEngine& mrcMatchingEngine;
};

#endif // TCORDERINTAKE_HPP