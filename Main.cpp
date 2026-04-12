#include "tcOrderBook.hpp"
#include "tcMatchingEngine.hpp"
#include "tcOrderIntake.hpp"
#include "constants.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ranges>

void showHelp()
{
    std::cout << "Commands:" << std::endl;
    std::cout << "  buy <quantity> <price> -- Submit a buy order" << std::endl;
    std::cout << "  sell <quantity> <price> - Submit a sell order" << std::endl;
    std::cout << "  show [depth] ------------ Show the order book (default depth is " << Constants::DEFAULT_PRINT_DEPTH << ")" << std::endl;
    std::cout << "  help -------------------- Show this help message" << std::endl;
    std::cout << "  exit -------------------- Exit the program" << std::endl;
}

void printLevel(uint32_t anPriceInTicks, const tcPriceLevel& lrcLevel)
{
    double lrPrice = anPriceInTicks * Constants::DOLLARS_PER_TICK;
    std::cout << std::setw(10) << lrcLevel.getTotalQuantity() << " @ $" <<
        std::fixed << std::setprecision(2) << lrPrice << std::endl;
}

void printOrderBook(tcOrderBook& lrcOrderBook, uint32_t anDepth = Constants::DEFAULT_PRINT_DEPTH)
{
    auto& lrcAsks = lrcOrderBook.getAsks();
    auto& lrcBids = lrcOrderBook.getBids();

    if (lrcAsks.empty() && lrcBids.empty())
    {
        std::cout << "Order book is empty." << std::endl;
        return;
    }

    std::cout << "\n======= ORDER BOOK =======\n" << std::endl;

    // Need to reverse asks after finding best levels since they are stored in
    // ascending order
    for (const auto& [lnTicks, lrcLevel] : lrcAsks | std::views::take(anDepth) | std::views::reverse)
    {
        printLevel(lnTicks, lrcLevel);
    }

    std::cout << "----------" << std::endl;

    for (const auto& [lnTicks, lrcLevel] : lrcBids | std::views::take(anDepth))
    {
        printLevel(lnTicks, lrcLevel);
    }

    // Show spread and market stock price (midpoint of best bid and ask)
    uint32_t lnBestAsk = lrcAsks.empty() ? 0 : lrcAsks.begin()->first;
    uint32_t lnBestBid = lrcBids.empty() ? 0 : lrcBids.begin()->first;
    if (lnBestAsk > 0 && lnBestBid > 0)
    {
        double lrSpread = (lnBestAsk - lnBestBid) * Constants::DOLLARS_PER_TICK;
        double lrMidpoint = (lnBestAsk + lnBestBid) * 0.5 * Constants::DOLLARS_PER_TICK;
        std::cout << "\nSpread: $" << std::fixed << std::setprecision(2) << lrSpread << std::endl;
        std::cout << "Market: $" << std::fixed << std::setprecision(2) << lrMidpoint << std::endl;
    }

    std::cout << "\n==========================" << std::endl;
}

int main()
{
    tcOrderBook lcOrderBook;
    tcMatchingEngine lcMatchingEngine(lcOrderBook);
    tcOrderIntake lcOrderIntake(lcOrderBook, lcMatchingEngine);

    uint64_t lnNextId = 1;

    std::cout << "Simple Order Book CLI" << std::endl;
    std::cout << "Type 'help' for a list of commands." << std::endl;

    std::string lcInput;

    while (true)
    {
        // Get user input
        std::cout << "> ";
        if (!std::getline(std::cin, lcInput) || lcInput == "exit")
        {
            break;
        }

        std::istringstream lcStream(lcInput);
        std::string lcCommand;
        lcStream >> lcCommand;

        if (lcCommand == "buy" || lcCommand == "sell")
        {
            uint32_t lnQuantity;
            double lrPrice;

            if (!(lcStream >> lnQuantity >> lrPrice))
            {
                std::cout << "Invalid command format. Usage: buy <quantity> <price> or sell <quantity> <price>" << std::endl;
                continue;
            }

            bool abIsBuy = (lcCommand == "buy");

            lcOrderIntake.submitOrder(lnNextId++, lnQuantity, lrPrice, abIsBuy);
        }
        else if (lcCommand == "show")
        {
            uint32_t lnDepth;
            if (lcStream >> lnDepth)
            {
                printOrderBook(lcOrderBook, lnDepth);
            }
            else
            {
                printOrderBook(lcOrderBook); // Default depth
            }
        }
        else if (lcCommand == "help")
        {
            showHelp();
        }
        else if (lcCommand.empty())
        {
            continue; // Ignore empty input
        }
        else
        {
            std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
        }
    }

    std::cout << "Exiting..." << std::endl;

    return 0;
}