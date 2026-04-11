#include "tcMatchingEngine.hpp"
#include "tcOrderBook.hpp"
#include "tcPriceLevel.hpp"

#include <gtest/gtest.h>

class tcOrderBookUT
{
public:
    static unsigned getNumOrders(const tcOrderBook& arcOrderBook)
    {
        return arcOrderBook.mcOrders.size();
    }

    static unsigned getNumBidLevels(const tcOrderBook& arcOrderBook)
    {
        return arcOrderBook.mcBids.size();
    }

    static unsigned getNumAskLevels(const tcOrderBook& arcOrderBook)
    {
        return arcOrderBook.mcAsks.size();
    }
};

class tcPriceLevelUT
{
public:
    static void setTotalQuantity(tcPriceLevel* apcPriceLevel, uint32_t anQuantity)
    {
        apcPriceLevel->mnTotalQuantity = anQuantity;
    }
};

class tcMatchingEngineUT : public testing::Test
{
public:
    static void CallExecuteTrade(
        tcMatchingEngine& arcMatchingEngine,
        tsOrder* apsTaker,
        tsOrder* apsMaker,
        uint32_t anQuantity,
        tcPriceLevel* apcPriceLevel)
    {
        arcMatchingEngine.executeTrade(
            apsTaker,
            apsMaker,
            anQuantity,
            apcPriceLevel);
    }

    static void CallMatch(
        tcMatchingEngine& arcMatchingEngine,
        tsOrder* apsIncomingOrder)
    {
        arcMatchingEngine.match(apsIncomingOrder);
    }
};

TEST(tcMatchingEngineUT, VerifyExecuteTrade)
{
    struct tsTestParams
    {
        std::string mcDesc;

        uint32_t mnTakerInitialQty;
        uint32_t mnMakerInitialQty;
        uint32_t mnTradeQty;
        uint32_t mnExpTakerRemaining;
        uint32_t mnExpMakerRemaining;
    };

    uint32_t lnPriceInTicks = 1000;  // Arbitrary

    std::vector<tsTestParams> lcTests = {
        // Desc                    TakerInitQty   MakerInitQty   TradeQty   ExpTakerRemaining   ExpMakerRemaining
        {"Full fill",              100,           100,           100,       0,                  0},
        {"Partial fill (taker)",   150,           100,           100,       50,                 0},
        {"Partial fill (maker)",   100,           150,           100,       0,                  50},
        {"Partial fill",           67,            88,            50,        17,                 38},
    };

    for (const auto& lrsTest : lcTests)
    {
        std::cout << "Test: " << lrsTest.mcDesc << std::endl;

        tcOrderBook lcOrderBook;
        tcMatchingEngine tcMatchingEngine(lcOrderBook);

        tsOrder* lpsTakerOrder =
            lcOrderBook.createOrder(1, lrsTest.mnTakerInitialQty, lnPriceInTicks, true);
        tsOrder* lpsMakerOrder =
            lcOrderBook.createOrder(2, lrsTest.mnMakerInitialQty, lnPriceInTicks, false);

        tcPriceLevel* lpcPriceLevel = new tcPriceLevel();
        tcPriceLevelUT::setTotalQuantity(lpcPriceLevel, lrsTest.mnMakerInitialQty);

        tcMatchingEngineUT::CallExecuteTrade(
            tcMatchingEngine,
            lpsTakerOrder,
            lpsMakerOrder,
            lrsTest.mnTradeQty,
            lpcPriceLevel);

        EXPECT_EQ(lpsTakerOrder->mnRemaining, lrsTest.mnExpTakerRemaining);
        EXPECT_EQ(lpsMakerOrder->mnRemaining, lrsTest.mnExpMakerRemaining);
        EXPECT_EQ(lpcPriceLevel->getTotalQuantity(), lrsTest.mnExpMakerRemaining);
    }
}

TEST(tcMatchingEngineUT, VerifyMatchAgainstEmptyBook)
{
    tcOrderBook lcOrderBook;
    tcMatchingEngine lcMatchingEngine(lcOrderBook);

    tsOrder* lpsBuyOrder = lcOrderBook.createOrder(1, 100, 1000, true);
    tsOrder* lpsSellOrder = lcOrderBook.createOrder(2, 100, 1000, false);

    tcMatchingEngineUT::CallMatch(lcMatchingEngine, lpsBuyOrder);
    EXPECT_EQ(lpsBuyOrder->mnRemaining, 100); // No match should occur

    tcMatchingEngineUT::CallMatch(lcMatchingEngine, lpsSellOrder);
    EXPECT_EQ(lpsSellOrder->mnRemaining, 100); // No match should occur
}

TEST(tcMatchingEngineUT, VerifyProcessIncomingSellOrder)
{
    struct tsTestParams
    {
        std::string mcDesc;

        uint32_t mnTakerInitialQty;
        uint32_t mnTakerPriceInTicks;
        uint32_t mnTradeQty;
        uint32_t mnExpPriceLevelTotalQty;
        unsigned mnExpectedRemainingBidLevels;
        unsigned mnExpectedRemainingAskLevels;
    };

    std::vector<tsTestParams> lcTests = {
        // Desc                    TakerInitQty   TakerPrice   TradeQty   ExpPriceLevelTotalQty   ExpNumBids   ExpNumAsks
        {"Full fill",              100,           1000,        100,       10,                     2,           0},
        {"Partial fill (taker)",   150,           1000,        100,       10,                     2,           1},
        {"Partial fill (maker)",   33,            999,         33,        67,                     3,           0},
        {"No fill",                100,           1001,        0,         100,                    3,           1},
        {"Multi-level fill 1",     200,           925,         110,       45,                     1,           1},
        {"Multi-level fill 2",     130,           900,         130,       25,                     1,           0},
        {"Multi-level fill 3",     200,           900,         155,       0,                      0,           1},
    };

    for (const auto& lrsTest : lcTests)
    {
        std::cout << "Test: " << lrsTest.mcDesc << std::endl;

        tcOrderBook orderBook;
        tcMatchingEngine matchingEngine(orderBook);

        // Populate book with bids (100 @ $10, 10 @ $9.5, 45 @ $9)
        tsOrder* order1 = orderBook.createOrder(1, 100, 1000, true);
        tsOrder* order2 = orderBook.createOrder(2, 10, 950, true);
        tsOrder* order3 = orderBook.createOrder(3, 15, 900, true);
        tsOrder* order4 = orderBook.createOrder(4, 30, 900, true);
        orderBook.addOrder(order1);
        orderBook.addOrder(order2);
        orderBook.addOrder(order3);
        orderBook.addOrder(order4);

        tsOrder* takerOrder = orderBook.createOrder(5, lrsTest.mnTakerInitialQty, lrsTest.mnTakerPriceInTicks, false);

        matchingEngine.process(takerOrder);

        // Verify remaining quantity for taker order
        // NOTE: Order could have been deleted, making takerOrder a dangling
        // pointer, so avoid quantity check if we expected a full fill
        if (lrsTest.mnTradeQty < lrsTest.mnTakerInitialQty)
        {
            EXPECT_EQ(takerOrder->mnRemaining, lrsTest.mnTakerInitialQty - lrsTest.mnTradeQty);
        }

        // Verify expected number of orders remaining on each side of the book
        EXPECT_EQ(tcOrderBookUT::getNumBidLevels(orderBook), lrsTest.mnExpectedRemainingBidLevels);
        EXPECT_EQ(tcOrderBookUT::getNumAskLevels(orderBook), lrsTest.mnExpectedRemainingAskLevels);

        // Verify remaining quantity at new best price level (if any)
        if (lrsTest.mnExpectedRemainingBidLevels > 0)
        {
            EXPECT_EQ(orderBook.getBestBid()->getTotalQuantity(), lrsTest.mnExpPriceLevelTotalQty);
        }
        else
        {
            EXPECT_EQ(orderBook.getBestBid(), nullptr);
        }
    }
}

TEST(tcMatchingEngineUT, VerifyProcessIncomingBuyOrder)
{
    struct tsTestParams
    {
        std::string mcDesc;

        uint32_t mnTakerInitialQty;
        uint32_t mnTakerPriceInTicks;
        uint32_t mnTradeQty;
        uint32_t mnExpPriceLevelTotalQty;
        unsigned mnExpectedRemainingBidLevels;
        unsigned mnExpectedRemainingAskLevels;
    };

    std::vector<tsTestParams> lcTests = {
        // Desc                    TakerInitQty   TakerPrice   TradeQty   ExpPriceLevelTotalQty   ExpNumBids   ExpNumAsks
        {"Full fill",              100,           900,         100,       10,                     0,           2},
        {"Partial fill (taker)",   150,           900,         100,       10,                     1,           2},
        {"Partial fill (maker)",   33,            901,         33,        67,                     0,           3},
        {"No fill",                100,           850,         0,         100,                    1,           3},
        {"Multi-level fill 1",     200,           975,         110,       45,                     1,           1},
        {"Multi-level fill 2",     130,           1000,        130,       25,                     0,           1},
        {"Multi-level fill 3",     200,           1000,        155,       0,                      1,           0},
    };

    for (const auto& lrsTest : lcTests)
    {
        std::cout << "Test: " << lrsTest.mcDesc << std::endl;

        tcOrderBook orderBook;
        tcMatchingEngine matchingEngine(orderBook);

        // Populate book with asks (100 @ $9, 10 @ $9.5, 45 @ $10)
        tsOrder* order1 = orderBook.createOrder(1, 100, 900, false);
        tsOrder* order2 = orderBook.createOrder(2, 10, 950, false);
        tsOrder* order3 = orderBook.createOrder(3, 15, 1000, false);
        tsOrder* order4 = orderBook.createOrder(4, 30, 1000, false);
        orderBook.addOrder(order1);
        orderBook.addOrder(order2);
        orderBook.addOrder(order3);
        orderBook.addOrder(order4);

        tsOrder* takerOrder = orderBook.createOrder(5, lrsTest.mnTakerInitialQty, lrsTest.mnTakerPriceInTicks, true);

        matchingEngine.process(takerOrder);

        // Verify remaining quantity for taker order
        // NOTE: Order could have been deleted, making takerOrder a dangling
        // pointer, so avoid quantity check if we expected a full fill
        if (lrsTest.mnTradeQty < lrsTest.mnTakerInitialQty)
        {
            EXPECT_EQ(takerOrder->mnRemaining, lrsTest.mnTakerInitialQty - lrsTest.mnTradeQty);
        }

        // Verify expected number of orders remaining on each side of the book
        EXPECT_EQ(tcOrderBookUT::getNumBidLevels(orderBook), lrsTest.mnExpectedRemainingBidLevels);
        EXPECT_EQ(tcOrderBookUT::getNumAskLevels(orderBook), lrsTest.mnExpectedRemainingAskLevels);

        // Verify remaining quantity at new best price level (if any)
        if (lrsTest.mnExpectedRemainingAskLevels > 0)
        {
            EXPECT_EQ(orderBook.getBestAsk()->getTotalQuantity(), lrsTest.mnExpPriceLevelTotalQty);
        }
        else
        {
            EXPECT_EQ(orderBook.getBestAsk(), nullptr);
        }
    }
}