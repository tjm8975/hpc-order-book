#include "tcOrderBook.hpp"
#include "tcMatchingEngine.hpp"
#include "tcOrderIntake.hpp"
#include "constants.hpp"

#include <iostream>
#include <sstream>
#include <string>

void showHelp()
{
    std::cout << "Commands:" << std::endl;
    std::cout << "  buy <quantity> <price> -- Submit a buy order" << std::endl;
    std::cout << "  sell <quantity> <price> - Submit a sell order" << std::endl;
    std::cout << "  cancel <order_id> ------- Cancel an order" << std::endl;
    std::cout << "  show book [depth] ------- Show the order book (default depth is " << Constants::DEFAULT_PRINT_DEPTH << ")" << std::endl;
    std::cout << "  show orders ------------- Show all active orders (in reverse chronological order)" << std::endl;
    std::cout << "  help -------------------- Show this help message" << std::endl;
    std::cout << "  exit -------------------- Exit the program" << std::endl;
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
        else if (lcCommand == "cancel")
        {
            uint64_t lnId;
            if (lcStream >> lnId)
            {
                if (!lcOrderIntake.cancelOrder(lnId))
                {
                    std::cout << "Order ID " << lnId << " not found for cancellation." << std::endl;
                }
            }
            else
            {
                std::cout << "Invalid command format. Usage: cancel <order_id>" << std::endl;
            }
        }
        else if (lcCommand == "show")
        {
            std::string lcSubCommand;
            lcStream >> lcSubCommand;
            if (lcSubCommand == "book")
            {
                uint32_t lnDepth;
                if (lcStream >> lnDepth)
                {
                    lcOrderBook.printOrderBook(lnDepth);
                }
                else
                {
                    lcOrderBook.printOrderBook(); // Default depth
                }
            }
            else if (lcSubCommand == "orders")
            {
                lcOrderBook.printOrders();
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