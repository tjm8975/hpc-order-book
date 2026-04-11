#include "tcPriceLevel.hpp"
#include "tcOrderBook.hpp"

#include <gtest/gtest.h>

class tcPriceLevelUT : public testing::Test
{
public:
    static tsOrder* getTail(tcPriceLevel* apcPriceLevel)
    {
        return apcPriceLevel->mpsTail;
    }
};

TEST(tcPriceLevelUT, VerifyAddAndRemoveOrder)
{
    struct tsTestParams
    {
        std::string mcDesc;

        bool mbAddOrder;
        uint64_t mnOrderId;
        uint32_t mnQuantity;
        uint32_t mnExpTotalQuantity;
        uint64_t mnExpHeadId;
        uint64_t mnExpTailId;
    };

    std::unordered_map<uint64_t, tsOrder*> lcOrders;
    tcPriceLevel lcPriceLevel;

    std::vector<tsTestParams> lcTests = {
        // Desc              Add      ID   Qty    ExpTotQty   ExpHead   ExpTail
        {"Add Order 1",      true,    1,   100,   100,        1,        1},
        {"Add Order 2",      true,    2,   50,    150,        1,        2},
        {"Remove Order 1",   false,   1,   0,     50,         2,        2},
        {"Remove Order 2",   false,   2,   0,     0,          0,        0},
        {"Add Order 3",      true,    3,   75,    75,         3,        3},
        {"Add Order 4",      true,    4,   95,    170,        3,        4},
        {"Add Order 5",      true,    5,   120,   290,        3,        5},
        {"Remove Order 4",   false,   4,   0,     195,        3,        5},
        {"Remove Order 5",   false,   5,   0,     75,         3,        3},
        {"Remove Order 3",   false,   3,   0,     0,          0,        0},
    };

    for (const auto& lrsTest : lcTests)
    {
        std::cout << "Test: " << lrsTest.mcDesc << std::endl;

        if (lrsTest.mbAddOrder)
        {
            tsOrder* lpsOrder = new tsOrder();
            lpsOrder->mnId = lrsTest.mnOrderId;
            lpsOrder->mnQuantity = lrsTest.mnQuantity;
            lpsOrder->mnRemaining = lrsTest.mnQuantity;

            lcOrders[lrsTest.mnOrderId] = lpsOrder;
            lcPriceLevel.add(lpsOrder);
        }
        else
        {
            auto lcIter = lcOrders.find(lrsTest.mnOrderId);
            ASSERT_NE(lcIter, lcOrders.end()) <<
                "Order ID " << lrsTest.mnOrderId << " not found for removal.";
            lcPriceLevel.remove(lcIter->second);
            delete lcIter->second;  // Clean up memory
            lcOrders.erase(lcIter);
        }

        EXPECT_EQ(lcPriceLevel.getTotalQuantity(), lrsTest.mnExpTotalQuantity);

        if (lrsTest.mnExpHeadId == 0)
        {
            EXPECT_TRUE(lcPriceLevel.isEmpty());
            EXPECT_EQ(lcPriceLevel.getBestOrder(), nullptr);
        }
        else
        {
            EXPECT_FALSE(lcPriceLevel.isEmpty());
            EXPECT_EQ(lcPriceLevel.getBestOrder(), lcOrders[lrsTest.mnExpHeadId]);
        }
        
        EXPECT_EQ(
            tcPriceLevelUT::getTail(&lcPriceLevel),
            lrsTest.mnExpTailId != 0 ? lcOrders[lrsTest.mnExpTailId] : nullptr);
    }
}