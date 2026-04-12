#include "tcOrderBook.hpp"

#include <gtest/gtest.h>

class tcOrderBookUT : public testing::Test
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

TEST(tcOrderBookUT, VerifyCreateAndAddOrder)
{
    struct tsTestParams
    {
        std::string mcDesc;

        uint64_t mnExpId;
        uint32_t mnExpQuantity;
        uint32_t mnExpPriceInTicks;
        bool     mbExpIsBuy;
        bool     mbExpExistingPriceLevel;
    };

    std::vector<tsTestParams> lcTests = {
        // Desc            Id   Quantity   PriceInTicks   IsBuy   ExistingPriceLevel
        {"Buy order 1",    1,   100,       10,            true,   true},
        {"Sell order 1",   2,   200,       20,            false,  true},
        {"Buy order 2",    3,   1500,      17,            true,   true},
        {"Sell order 2",   4,   2500,      86,            false,  true},
        {"Buy order 3",    5,   1,         185,           true,   true},
        {"Sell order 3",   6,   1,         200,           false,  true},
        {"Buy order 3.1",  7,   500,       185,           true,   false},
        {"Sell order 3.1", 8,   500,       200,           false,  false},
    };

    tcOrderBook lcOrderBook;

    for (const auto& lrsTest : lcTests)
    {
        std::cout << "Test: " << lrsTest.mcDesc << std::endl;

        unsigned lnInitialNumOrders = tcOrderBookUT::getNumOrders(lcOrderBook);
        unsigned lnInitialNumBidLevels = tcOrderBookUT::getNumBidLevels(lcOrderBook);
        unsigned lnInitialNumAskLevels = tcOrderBookUT::getNumAskLevels(lcOrderBook);

        tsOrder* order =
            lcOrderBook.createOrder(
                lrsTest.mnExpId,
                lrsTest.mnExpQuantity,
                lrsTest.mnExpPriceInTicks,
                lrsTest.mbExpIsBuy);
        lcOrderBook.addOrder(order);

        EXPECT_EQ(order->mnId, lrsTest.mnExpId);
        EXPECT_EQ(order->mnQuantity, lrsTest.mnExpQuantity);
        EXPECT_EQ(order->mnRemaining, lrsTest.mnExpQuantity);
        EXPECT_EQ(order->mnPriceInTicks, lrsTest.mnExpPriceInTicks);
        EXPECT_EQ(order->mbIsBuy, lrsTest.mbExpIsBuy);

        EXPECT_EQ(tcOrderBookUT::getNumOrders(lcOrderBook), lnInitialNumOrders + 1);
        if (lrsTest.mbExpIsBuy)
        {
            EXPECT_EQ(tcOrderBookUT::getNumBidLevels(lcOrderBook),
                      lnInitialNumBidLevels + lrsTest.mbExpExistingPriceLevel);
            EXPECT_EQ(tcOrderBookUT::getNumAskLevels(lcOrderBook), lnInitialNumAskLevels);
        }
        else
        {
            EXPECT_EQ(tcOrderBookUT::getNumBidLevels(lcOrderBook), lnInitialNumBidLevels);
            EXPECT_EQ(tcOrderBookUT::getNumAskLevels(lcOrderBook),
                      lnInitialNumAskLevels + lrsTest.mbExpExistingPriceLevel);
        }
    }
}

TEST(tcOrderBookUT, VerifyGetBestBidAndAsk)
{
    struct tsTestParams
    {
        std::string mcDesc;

        std::vector<uint32_t> mcBidPrices;
        std::vector<uint32_t> mcAskPrices;
        uint64_t mnExpBestBidId;
        uint64_t mnExpBestAskId;
    };

    std::vector<tsTestParams> lcTests = {
        // Desc               BidPrices       AskPrices       ExpBestBidId   ExpBestAskId
        {"Empty book",        {},             {},             0,             0},
        {"Only bids",         {10, 20, 15},   {},             2,             0},
        {"Only asks",         {},             {30, 25, 35},   0,             2},
        {"Bids and asks",     {10, 15, 20},   {30, 25, 35},   3,             5},
        {"Same price levels", {10, 20, 20},   {25, 25, 30},   2,             4},
    };

    for (const auto& lrsTest : lcTests)
    {
        std::cout << "Test: " << lrsTest.mcDesc << std::endl;

        tcOrderBook lcOrderBook;
        uint64_t lnOrderId = 1;
        uint32_t lnQuantity = 100;  // Arbitrary

        for (uint32_t price : lrsTest.mcBidPrices)
        {
            tsOrder* order = lcOrderBook.createOrder(lnOrderId++, lnQuantity, price, true);
            lcOrderBook.addOrder(order);
        }
        for (uint32_t price : lrsTest.mcAskPrices)
        {
            tsOrder* order = lcOrderBook.createOrder(lnOrderId++, lnQuantity, price, false);
            lcOrderBook.addOrder(order);
        }

        if (lrsTest.mnExpBestBidId != 0)
        {
            EXPECT_EQ(lcOrderBook.getBestBid()->getBestOrder()->mnId, lrsTest.mnExpBestBidId);
        }
        else
        {
            EXPECT_EQ(lcOrderBook.getBestBid(), nullptr);
        }

        if (lrsTest.mnExpBestAskId != 0)
        {
            EXPECT_EQ(lcOrderBook.getBestAsk()->getBestOrder()->mnId, lrsTest.mnExpBestAskId);
        }
        else
        {
            EXPECT_EQ(lcOrderBook.getBestAsk(), nullptr);
        }
    }
}

TEST(tcOrderBookUT, VerifyRemoveOrder)
{
    struct tsTestParams
    {
        std::string mcDesc;

        uint64_t mnIdToRemove;
        unsigned mnExpNumOrders;
        uint32_t mnExpBestBidQty;
        uint32_t mnExpBestAskQty;
        bool     mbRemoveFromPriceLevel;
    };

    std::vector<tsTestParams> lcTests = {
        // Desc                  IdToRemove   ExpNumOrders   ExpBestBidQty   ExpBestAskQty   RemoveFromPriceLevel
        {"Non-existent order",   999,         4,             300,            200,            true},
        {"Best ask",             2,           3,             300,            500,            true},
        {"Best bid",             3,           2,             200,            500,            true},
        {"Remaining bid",        1,           1,             0,              500,            true},
        {"Keep level",           4,           0,             0,              500,            false},
    };

    // Populate order book with multiple orders at different price levels
    tcOrderBook lcOrderBook;
    tsOrder* lpsOrder1 = lcOrderBook.createOrder(1, 200, 19, true);
    lcOrderBook.addOrder(lpsOrder1);
    tsOrder* lpsOrder2 = lcOrderBook.createOrder(2, 200, 20, false);
    lcOrderBook.addOrder(lpsOrder2);
    tsOrder* lpsOrder3 = lcOrderBook.createOrder(3, 100, 19, true);
    lcOrderBook.addOrder(lpsOrder3);
    tsOrder* lpsOrder4 = lcOrderBook.createOrder(4, 500, 21, false);
    lcOrderBook.addOrder(lpsOrder4);

    for (const auto& lrsTest : lcTests)
    {
        std::cout << "Test: " << lrsTest.mcDesc << std::endl;
        lcOrderBook.removeOrder(lrsTest.mnIdToRemove, lrsTest.mbRemoveFromPriceLevel);
        EXPECT_EQ(tcOrderBookUT::getNumOrders(lcOrderBook), lrsTest.mnExpNumOrders);

        tcPriceLevel* lpcBestBid = lcOrderBook.getBestBid();
        if (lpcBestBid != nullptr)
        {
            EXPECT_EQ(lpcBestBid->getTotalQuantity(), lrsTest.mnExpBestBidQty);
        }
        else
        {
            EXPECT_EQ(lrsTest.mnExpBestBidQty, 0);
        }

        tcPriceLevel* lpcBestAsk = lcOrderBook.getBestAsk();
        if (lpcBestAsk != nullptr)
        {
            EXPECT_EQ(lpcBestAsk->getTotalQuantity(), lrsTest.mnExpBestAskQty);
        }
        else
        {
            EXPECT_EQ(lrsTest.mnExpBestAskQty, 0);
        }
    }
}

TEST(tcOrderBookUT, VerifyCancelOrder)
{
    tcOrderBook lcOrderBook;
    tsOrder* lpsOrder1 = lcOrderBook.createOrder(1, 200, 19, true);
    lcOrderBook.addOrder(lpsOrder1);
    tsOrder* lpsOrder2 = lcOrderBook.createOrder(2, 200, 20, false);
    lcOrderBook.addOrder(lpsOrder2);

    EXPECT_TRUE(lcOrderBook.cancelOrder(1));
    EXPECT_EQ(tcOrderBookUT::getNumOrders(lcOrderBook), 1);
    EXPECT_EQ(lcOrderBook.getBestBid(), nullptr); // Best bid was cancelled

    EXPECT_TRUE(lcOrderBook.cancelOrder(2));
    EXPECT_EQ(tcOrderBookUT::getNumOrders(lcOrderBook), 0);
    EXPECT_EQ(lcOrderBook.getBestAsk(), nullptr); // Best ask was cancelled

    EXPECT_FALSE(lcOrderBook.cancelOrder(999)); // Non-existent order
}