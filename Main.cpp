#include "tcOrderBook.hpp"
#include "tcMatchingEngine.hpp"
#include "tcOrderIntake.hpp"
#include "constants.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

uint64_t gnNextId = 1;

void showHelp()
{
    std::cout << "Commands:" << std::endl;
    std::cout << "  buy <quantity> <price> -- Submit a buy order" << std::endl;
    std::cout << "  sell <quantity> <price> - Submit a sell order" << std::endl;
    std::cout << "  cancel <order_id> ------- Cancel an order" << std::endl;
    std::cout << "  load <relative_path> ---- Load commands from a file" << std::endl;
    std::cout << "  show book [depth] ------- Show the order book (default depth is " << Constants::DEFAULT_PRINT_DEPTH << ")" << std::endl;
    std::cout << "  show orders ------------- Show all active orders (in reverse chronological order)" << std::endl;
    std::cout << "  help -------------------- Show this help message" << std::endl;
    std::cout << "  exit -------------------- Exit the program" << std::endl;
}

void handleCommand(
    std::istringstream& arcStream,
    std::string& arcCommand,
    tcOrderIntake& arcOrderIntake,
    tcOrderBook& arcOrderBook)
{
    if (arcCommand.empty())
    {
        return; // Ignore empty input
    }
    else if (arcCommand == "buy" || arcCommand == "sell")
    {
        uint32_t lnQuantity;
        double lrPrice;

        if (!(arcStream >> lnQuantity >> lrPrice))
        {
            std::cout << "Invalid command format. Usage: buy <quantity> <price> or sell <quantity> <price>" << std::endl;
            return;
        }

        bool abIsBuy = (arcCommand == "buy");

        arcOrderIntake.submitOrder(gnNextId++, lnQuantity, lrPrice, abIsBuy);
    }
    else if (arcCommand == "cancel")
    {
        uint64_t lnId;
        if (arcStream >> lnId)
        {
            if (!arcOrderIntake.cancelOrder(lnId))
            {
                std::cout << "Order ID " << lnId << " not found for cancellation." << std::endl;
            }
        }
        else
        {
            std::cout << "Invalid command format. Usage: cancel <order_id>" << std::endl;
        }
    }
    else if (arcCommand == "show")
    {
        std::string lcSubCommand;
        arcStream >> lcSubCommand;
        if (lcSubCommand == "book")
        {
            uint32_t lnDepth;
            if (arcStream >> lnDepth)
            {
                arcOrderBook.printOrderBook(lnDepth);
            }
            else
            {
                arcOrderBook.printOrderBook(); // Default depth
            }
        }
        else if (lcSubCommand == "orders")
        {
            arcOrderBook.printOrders();
        }
    }
    else if (arcCommand == "help")
    {
        showHelp();
    }
    else
    {
        std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
    }
}

void loadCommandsFromFile(
    std::istringstream& arcStream,
    tcOrderIntake& arcOrderIntake,
    tcOrderBook& arcOrderBook)
{
    std::string lcFilename;
    if (arcStream >> lcFilename)
    {
        std::ifstream lcFile(lcFilename);
        if (!lcFile)
        {
            std::cout << "Failed to open file: " << lcFilename << std::endl;
            return;
        }

        std::string lcFileLine;
        while (std::getline(lcFile, lcFileLine))
        {
            std::istringstream lcFileStream(lcFileLine);
            std::string lcFileCommand;
            lcFileStream >> lcFileCommand;

            handleCommand(lcFileStream, lcFileCommand, arcOrderIntake, arcOrderBook);
        }
    }
    else
    {
        std::cout << "Invalid command format. Usage: load <relative_path>" << std::endl;
    }
}

int main()
{
    tcOrderBook lcOrderBook;
    tcMatchingEngine lcMatchingEngine(lcOrderBook);
    tcOrderIntake lcOrderIntake(lcOrderBook, lcMatchingEngine);

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

        if (lcCommand == "load")
        {
            loadCommandsFromFile(lcStream, lcOrderIntake, lcOrderBook);
        }
        else
        {
            handleCommand(lcStream, lcCommand, lcOrderIntake, lcOrderBook);
        }
    }

    std::cout << "Exiting..." << std::endl;

    return 0;
}