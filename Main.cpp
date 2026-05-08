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
    std::cout << "   buy <exec_type> <quantity> [price_in_usd]" << std::endl;
    std::cout << "      Submit a Market buy order. Limit order if price is provided" << std::endl;
    std::cout << "      <exec_type> - Order execution constraint" << std::endl;
    std::cout << "         gtc - Good Til' Canceled" << std::endl;
    std::cout << "         ioc - Immediate Or Cancel" << std::endl;
    std::cout << "         fok - Fill Or Kill" << std::endl;
    std::cout << "      <quantity> - Quantity of shares you want to buy" << std::endl;
    std::cout << "      [price_in_usd] - Optional limit (maximum) price you are willing to pay\n" << std::endl;

    std::cout << "   sell <exec type> <quantity> [price_in_usd]" << std::endl;
    std::cout << "      Submit a Market sell order. Limit order if price is provided" << std::endl;
    std::cout << "      <exec_type> - Order execution constraint" << std::endl;
    std::cout << "         gtc - Good Til' Canceled" << std::endl;
    std::cout << "         ioc - Immediate Or Cancel" << std::endl;
    std::cout << "         fok - Fill Or Kill" << std::endl;
    std::cout << "      <quantity> - Quantity of shares you want to sell" << std::endl;
    std::cout << "      [price_in_usd] - Optional limit (minimum) price you are willing to accept\n" << std::endl;

    std::cout << "   cancel <order_id>" << std::endl;
    std::cout << "      Cancel the order with specified ID\n" << std::endl;

    std::cout << "   load <relative_path>" << std::endl;
    std::cout << "      Load commands from a file\n" << std::endl;

    std::cout << "   show book [depth]" << std::endl;
    std::cout << "      Show the order book (default depth is " << Constants::DEFAULT_PRINT_DEPTH << ")\n" << std::endl;

    std::cout << "   show orders" << std::endl;
    std::cout << "      Show all active orders (in reverse chronological order)\n" << std::endl;

    std::cout << "   reset" << std::endl;
    std::cout << "      Cancel all active orders and reset the order book\n" << std::endl;

    std::cout << "   help" << std::endl;
    std::cout << "      Show this help message\n" << std::endl;

    std::cout << "   exit" << std::endl;
    std::cout << "      Exit the program\n" << std::endl;
}

bool validateExecutionType(
    std::string & arcExecString,
    teExecType & areExecType,
    teOrderType & areOrderType)
{
    if (arcExecString == "gtc")
    {
        areExecType = teExecType::eeGoodTilCanceled;
        if (areOrderType == teOrderType::eeMarket)
        {
            std::cout << "ERROR: Good-til-Canceled execution type not supported for Market orders" << std::endl;
            return false;
        }
    }
    else if (arcExecString == "ioc")
    {
        areExecType = teExecType::eeImmediateOrCancel;
    }
    else if (arcExecString == "fok")
    {
        areExecType = teExecType::eeFillOrKill;
    }
    else
    {
        std::cout << "ERROR: Execution type '" << arcExecString << "' not found" << std::endl;
        return false;
    }

    return true;
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
        std::string lcExecString;
        uint32_t lnQuantity;
        double lrPrice;
        teOrderType leOrderType = teOrderType::eeLimit;
        teExecType leExecType;

        if (!(arcStream >> lcExecString) || !(arcStream >> lnQuantity))
        {
            std::cout << "ERROR: Invalid command format. Usage: buy <exec_type> <quantity> [price] or sell <exec_type> <quantity> [price]" << std::endl;
            return;
        }

        if (!(arcStream >> lrPrice))
        {
            // Treat as market order
            leOrderType = teOrderType::eeMarket;
            lrPrice = 0.0;
        }

        if (!validateExecutionType(lcExecString, leExecType, leOrderType))
        {
            return;
        }

        bool abIsBuy = (arcCommand == "buy");

        std::cout << "Submitted " << execTypeToString(leExecType) << " " << orderTypeToString(leOrderType) << (abIsBuy ? " Buy" : " Sell") << " order for " << lnQuantity << " shares";
        if (leOrderType == teOrderType::eeLimit)
        {
            std::cout <<  " @ $" << lrPrice;
        }
        std::cout << std::endl;

        arcOrderIntake.submitOrder(gnNextId++, lnQuantity, lrPrice, abIsBuy, leOrderType, leExecType);
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
            else
            {
                std::cout << "Cancelled order " << lnId << std::endl;
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
    else if (arcCommand == "reset")
    {
        arcOrderBook.reset();
        std::cout << "Order book reset. All active orders cancelled." << std::endl;
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