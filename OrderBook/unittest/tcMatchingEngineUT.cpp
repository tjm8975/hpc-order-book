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

TEST(tcMatchingEngineUT, VerifyProcessIncomingLimitSellOrder)
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

    std::vector<teExecType> lcExecTypes = {
        teExecType::eeGoodTilCanceled,
        teExecType::eeImmediateOrCancel,
    };

    for (teExecType leExecType : lcExecTypes)
    {
        std::cout << "Exec Type: " << execTypeToString(leExecType) << std::endl;
        for (const auto& lrsTest : lcTests)
        {
            std::cout << "Test: " << lrsTest.mcDesc << std::endl;

            tcOrderBook lcOrderBook;
            tcMatchingEngine lcMatchingEngine(lcOrderBook);

            // Populate book with bids (100 @ $10, 10 @ $9.5, 45 @ $9)
            tsOrder* lpsOrder1 = lcOrderBook.createOrder(1, 100, 1000, true);
            tsOrder* lpsOrder2 = lcOrderBook.createOrder(2, 10, 950, true);
            tsOrder* lpsOrder3 = lcOrderBook.createOrder(3, 15, 900, true);
            tsOrder* lpsOrder4 = lcOrderBook.createOrder(4, 30, 900, true);
            lcOrderBook.addOrder(lpsOrder1);
            lcOrderBook.addOrder(lpsOrder2);
            lcOrderBook.addOrder(lpsOrder3);
            lcOrderBook.addOrder(lpsOrder4);

            tsOrder* lpsTakerOrder =
                lcOrderBook.createOrder(
                    5,
                    lrsTest.mnTakerInitialQty,
                    lrsTest.mnTakerPriceInTicks,
                    false,
                    teOrderType::eeLimit,
                    leExecType);

            lcMatchingEngine.process(lpsTakerOrder);

            // Verify remaining quantity for taker order
            // NOTE: Order could have been deleted, making takerOrder a dangling
            // pointer, so avoid quantity check if we expected a full fill
            if (leExecType == teExecType::eeGoodTilCanceled &&
                lrsTest.mnTradeQty < lrsTest.mnTakerInitialQty)
            {
                EXPECT_EQ(lpsTakerOrder->mnRemaining, lrsTest.mnTakerInitialQty - lrsTest.mnTradeQty);
            }

            unsigned lnExpNumRemainingAsks =
                leExecType == teExecType::eeImmediateOrCancel ?
                    0 : lrsTest.mnExpectedRemainingAskLevels;

            // Verify expected number of orders remaining on each side of the book
            EXPECT_EQ(tcOrderBookUT::getNumBidLevels(lcOrderBook), lrsTest.mnExpectedRemainingBidLevels);
            EXPECT_EQ(tcOrderBookUT::getNumAskLevels(lcOrderBook), lnExpNumRemainingAsks);

            // Verify remaining quantity at new best price level (if any)
            if (lrsTest.mnExpectedRemainingBidLevels > 0)
            {
                EXPECT_EQ(lcOrderBook.getBestBid()->getTotalQuantity(), lrsTest.mnExpPriceLevelTotalQty);
            }
            else
            {
                EXPECT_EQ(lcOrderBook.getBestBid(), nullptr);
            }
        }
    }
}

TEST(tcMatchingEngineUT, VerifyProcessIncomingLimitBuyOrder)
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

    std::vector<teExecType> lcExecTypes = {
        teExecType::eeGoodTilCanceled,
        teExecType::eeImmediateOrCancel,
    };

    for (teExecType leExecType : lcExecTypes)
    {
        std::cout << "Exec Type: " << execTypeToString(leExecType) << std::endl;
        for (const auto& lrsTest : lcTests)
        {
            std::cout << "Test: " << lrsTest.mcDesc << std::endl;

            tcOrderBook lcOrderBook;
            tcMatchingEngine lcMatchingEngine(lcOrderBook);

            // Populate book with asks (100 @ $9, 10 @ $9.5, 45 @ $10)
            tsOrder* lpsOrder1 = lcOrderBook.createOrder(1, 100, 900, false);
            tsOrder* lpsOrder2 = lcOrderBook.createOrder(2, 10, 950, false);
            tsOrder* lpsOrder3 = lcOrderBook.createOrder(3, 15, 1000, false);
            tsOrder* lpsOrder4 = lcOrderBook.createOrder(4, 30, 1000, false);
            lcOrderBook.addOrder(lpsOrder1);
            lcOrderBook.addOrder(lpsOrder2);
            lcOrderBook.addOrder(lpsOrder3);
            lcOrderBook.addOrder(lpsOrder4);

            tsOrder* lpsTakerOrder =
                lcOrderBook.createOrder(
                    5,
                    lrsTest.mnTakerInitialQty,
                    lrsTest.mnTakerPriceInTicks,
                    true,
                    teOrderType::eeLimit,
                    leExecType);

            lcMatchingEngine.process(lpsTakerOrder);

            // Verify remaining quantity for taker order
            // NOTE: Order could have been deleted, making takerOrder a dangling
            // pointer, so avoid quantity check if we expected a full fill
            if (leExecType == teExecType::eeGoodTilCanceled &&
                lrsTest.mnTradeQty < lrsTest.mnTakerInitialQty)
            {
                EXPECT_EQ(lpsTakerOrder->mnRemaining, lrsTest.mnTakerInitialQty - lrsTest.mnTradeQty);
            }

            unsigned lnExpNumRemainingBids =
                leExecType == teExecType::eeImmediateOrCancel ?
                    0 : lrsTest.mnExpectedRemainingBidLevels;

            // Verify expected number of orders remaining on each side of the book
            EXPECT_EQ(tcOrderBookUT::getNumBidLevels(lcOrderBook), lnExpNumRemainingBids);
            EXPECT_EQ(tcOrderBookUT::getNumAskLevels(lcOrderBook), lrsTest.mnExpectedRemainingAskLevels);

            // Verify remaining quantity at new best price level (if any)
            if (lrsTest.mnExpectedRemainingAskLevels > 0)
            {
                EXPECT_EQ(lcOrderBook.getBestAsk()->getTotalQuantity(), lrsTest.mnExpPriceLevelTotalQty);
            }
            else
            {
                EXPECT_EQ(lcOrderBook.getBestAsk(), nullptr);
            }
        }
    }
}

TEST(tcMatchingEngineUT, VerifyProcessMarketOrder)
{
    struct tsTestParams
    {
        std::string mcDesc;

        uint32_t mnTakerInitialQty;
        uint32_t mnExpPriceLevelTotalQty;
        unsigned mnExpectedRemainingLevels;
    };

    std::vector<tsTestParams> lcTests = {
        // Desc                       TakerInitQty   ExpPriceLevelTotalQty   ExpNumLevels
        {"Full fill",                 100,           10,                     2},
        {"Partial fill",              200,           0,                      0},
        {"Fill at multiple levels",   150,           5,                      1},
    };

    // Test both sell and buy side
    for (int lnSide = 0; lnSide < 2; lnSide++)
    {
        std::cout << (lnSide == 0 ? "Sell" : "Buy") << " Side" << std::endl;
        bool lbIsIncomingBuy = lnSide;
        for (const auto& lrsTest : lcTests)
        {
            std::cout << "Test: " << lrsTest.mcDesc << std::endl;

            tcOrderBook lcOrderBook;
            tcMatchingEngine lcMatchingEngine(lcOrderBook);

            tsOrder * lpsOrder1 = nullptr;
            tsOrder * lpsOrder2 = nullptr;
            tsOrder * lpsOrder3 = nullptr;
            tsOrder * lpsOrder4 = nullptr;

            if (lbIsIncomingBuy)
            {
                // Populate book with asks (100 @ $9, 10 @ $9.5, 45 @ $10)
                lpsOrder1 = lcOrderBook.createOrder(1, 100, 900, false);
                lpsOrder2 = lcOrderBook.createOrder(2, 10, 950, false);
                lpsOrder3 = lcOrderBook.createOrder(3, 15, 1000, false);
                lpsOrder4 = lcOrderBook.createOrder(4, 30, 1000, false);
            }
            else
            {
                // Populate book with asks (100 @ $10, 10 @ $9.5, 45 @ $9)
                lpsOrder1 = lcOrderBook.createOrder(1, 100, 1000, true);
                lpsOrder2 = lcOrderBook.createOrder(2, 10, 950, true);
                lpsOrder3 = lcOrderBook.createOrder(3, 15, 900, true);
                lpsOrder4 = lcOrderBook.createOrder(4, 30, 900, true);
            }
            
            lcOrderBook.addOrder(lpsOrder1);
            lcOrderBook.addOrder(lpsOrder2);
            lcOrderBook.addOrder(lpsOrder3);
            lcOrderBook.addOrder(lpsOrder4);

            tsOrder* lpsTakerOrder =
                lcOrderBook.createOrder(
                    5,  // OrderId
                    lrsTest.mnTakerInitialQty,
                    0,  // Price (not applicable for market order)
                    lbIsIncomingBuy,
                    teOrderType::eeMarket,
                    teExecType::eeImmediateOrCancel);

            lcMatchingEngine.process(lpsTakerOrder);

            // Verify expected number of orders remaining on each side of the book
            int lnExpNumRemainingBids = lbIsIncomingBuy ? 0 : lrsTest.mnExpectedRemainingLevels;
            int lnExpNumRemainingAsks = lbIsIncomingBuy ? lrsTest.mnExpectedRemainingLevels : 0;
            EXPECT_EQ(tcOrderBookUT::getNumBidLevels(lcOrderBook), lnExpNumRemainingBids);
            EXPECT_EQ(tcOrderBookUT::getNumAskLevels(lcOrderBook), lnExpNumRemainingAsks);


            // Verify remaining quantity at new best price level (if any)
            if (!lbIsIncomingBuy && lrsTest.mnExpectedRemainingLevels > 0)
            {
                EXPECT_EQ(lcOrderBook.getBestBid()->getTotalQuantity(), lrsTest.mnExpPriceLevelTotalQty);
            }
            else if (lbIsIncomingBuy && lrsTest.mnExpectedRemainingLevels > 0)
            {
                EXPECT_EQ(lcOrderBook.getBestAsk()->getTotalQuantity(), lrsTest.mnExpPriceLevelTotalQty);
            }
        }
    }
}

TEST(tcMatchingEngineUT, VerifyMarketOrderOnEmptyBook)
{
    std::vector<int> lcQuantities = {10, 20, 100, 500};

    tcOrderBook lcOrderBook;
    tcMatchingEngine lcMatchingEngine(lcOrderBook);

    // Test both sell and buy side
    for (int lnSide = 0; lnSide < 2; lnSide++)
    {
        std::cout << (lnSide == 0 ? "Sell" : "Buy") << " Side" << std::endl;
        bool lbIsIncomingBuy = lnSide;

        for (int lnQty : lcQuantities)
        {
            std::cout << "Qty: " << lnQty << std::endl;
            tsOrder* lpsMarketOrder =
                lcOrderBook.createOrder(
                    1,  // OrderId
                    lnQty,
                    0,  // Price (not applicable for market order)
                    lbIsIncomingBuy,
                    teOrderType::eeMarket,
                    teExecType::eeImmediateOrCancel);

            lcMatchingEngine.process(lpsMarketOrder);

            // Verify expected number of orders remaining on each side of the book
            EXPECT_EQ(tcOrderBookUT::getNumBidLevels(lcOrderBook), 0);
            EXPECT_EQ(tcOrderBookUT::getNumAskLevels(lcOrderBook), 0);

            // Verify no quantity traded
            EXPECT_EQ(lpsMarketOrder->mnRemaining, lnQty);
        }
    }
}

TEST(tcMatchingEngineUT, VerifyGtcMarketOrderFailure)
{
    // Test both sell and buy side
    for (int lnSide = 0; lnSide < 2; lnSide++)
    {
        std::cout << (lnSide == 0 ? "Sell" : "Buy") << " Side" << std::endl;
        bool lbIsIncomingBuy = lnSide;

        tcOrderBook lcOrderBook;
        tcMatchingEngine lcMatchingEngine(lcOrderBook);

        // Dummy orders that market order would match on
        tsOrder * lpsOrder1 = lcOrderBook.createOrder(1, 100, 900, true);
        tsOrder * lpsOrder2 = lcOrderBook.createOrder(2, 10, 950, false);
        lcOrderBook.addOrder(lpsOrder1);
        lcOrderBook.addOrder(lpsOrder2);

        tsOrder* lpsMarketOrder =
            lcOrderBook.createOrder(
                3,  // OrderId
                200,
                0,  // Price (not applicable for market order)
                lbIsIncomingBuy,
                teOrderType::eeMarket,
                teExecType::eeGoodTilCanceled);

        lcMatchingEngine.process(lpsMarketOrder);

        // Verify expected number of orders remaining on each side of the book
        EXPECT_EQ(tcOrderBookUT::getNumBidLevels(lcOrderBook), 1);
        EXPECT_EQ(tcOrderBookUT::getNumAskLevels(lcOrderBook), 1);
    }
}