#ifndef TCMATCHINGENGINE_HPP
#define TCMATCHINGENGINE_HPP

#include "tcOrderBook.hpp"

class tcMatchingEngine
{
public:
    explicit tcMatchingEngine(tcOrderBook& arcOrderBook);

    void process(tsOrder* apsIncomingOrder);

private:
    tcOrderBook& mrcOrderBook;

    void match(tsOrder* apsIncomingOrder);

    void executeTrade(
        tsOrder* apsTaker,
        tsOrder* apsMaker,
        uint32_t anQuantity,
        tcPriceLevel* apcPriceLevel);

    // Grant access to private members for unit testing
    friend class tcMatchingEngineUT;
};

#endif // TCMATCHINGENGINE_HPP