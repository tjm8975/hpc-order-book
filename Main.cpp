#include "tcOrderBook.hpp"
#include "tcMatchingEngine.hpp"
#include "tcOrderIntake.hpp"

int main()
{
    tcOrderBook lcOrderBook;
    tcMatchingEngine lcMatchingEngine(lcOrderBook);
    tcOrderIntake lcOrderIntake(lcOrderBook, lcMatchingEngine);

    lcOrderIntake.submitOrder(1, 100, 100, true); // Buy 100 @ $100.00
    lcOrderIntake.submitOrder(2, 100, 105, false);  // Sell 100 @ $105.00

    lcOrderBook.printOrderBook(); // For debugging purposes

    lcOrderIntake.submitOrder(3, 50, 105, true); // Buy 50 @ $105.00
    lcOrderIntake.submitOrder(4, 50, 100, false); // Sell 50 @ $100.00

    lcOrderBook.printOrderBook(); // For debugging purposes

    lcOrderIntake.submitOrder(5, 75, 105.01, true); // Buy 75 @ $105.01

    lcOrderBook.printOrderBook(); // For debugging purposes

    return 0;
}