#include "tcOrderIntake.hpp"
#include "constants.hpp"

tcOrderIntake::tcOrderIntake(tcOrderBook& arcOrderBook, tcMatchingEngine& arcMatchingEngine) :
    mrcOrderBook(arcOrderBook),
    mrcMatchingEngine(arcMatchingEngine)
{
}

void tcOrderIntake::submitOrder(
    uint64_t anId,
    uint32_t anQuantity,
    double arPrice,
    bool abIsBuy,
    teOrderType aeOrderType,
    teExecType aeExecType)
{
    tsOrder* lpsNewOrder =
        mrcOrderBook.createOrder(
            anId,
            anQuantity,
            arPrice * Constants::TICKS_PER_DOLLAR,
            abIsBuy,
            aeOrderType,
            aeExecType);
    mrcMatchingEngine.process(lpsNewOrder);
}

bool tcOrderIntake::cancelOrder(uint64_t anId)
{
    return mrcOrderBook.cancelOrder(anId);
}