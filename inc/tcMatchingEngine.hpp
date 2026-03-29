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

    void executeTrade(tsOrder* apsTaker, tsOrder* apsMaker, uint32_t anQuantity);
};

#endif // TCMATCHINGENGINE_HPP