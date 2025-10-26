#include <iostream>
#include <Windows.h>
#include <cstdlib>

int ErrorExit(const std::string& message)
{
    std::cerr << message << " (error code: " << GetLastError() << ")" << std::endl;
    system("pause");
    return EXIT_FAILURE;
}

void PrintState(const FILTERKEYS& keys)
{
    //std::cout << "\tAcceptance Delay: " << keys.iWaitMSec << std::endl;
    std::cout << "\tDelay Until Repeat: " << keys.iDelayMSec << std::endl;
    std::cout << "\tRepeat Rate: " << keys.iRepeatMSec << std::endl;
    //std::cout << "\tDebounce Time: " << keys.iBounceMSec << std::endl;
}

int main(int argc, char* argv[])
{
    BOOL success;

    // Get current FilterKeys struct
    FILTERKEYS keys;
    ZeroMemory(&keys, sizeof(FILTERKEYS));
    keys.cbSize = sizeof(FILTERKEYS);
    success = SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &keys, 0);         
    if (!success)
        return ErrorExit("Failed to get current FilterKeys struct");

    std::cout << "Current State: " << std::endl;
    PrintState(keys);
    std::cout << std::endl;

    // Input new delay and repeat rate
    std::cout << "Enter new delay until repeat: ";
    std::cin >> keys.iDelayMSec;
    if (std::cin.fail())
        return ErrorExit("Invalid input");

    std::cout << "Enter new repeat rate: ";
    std::cin >> keys.iRepeatMSec;
    if (std::cin.fail())
        return ErrorExit("Invalid input");

    std::cout << std::endl;
    std::cout << "Setting new state:" << std::endl;
    PrintState(keys);

    // Necessary flags
    keys.dwFlags |= FKF_FILTERKEYSON;
    keys.dwFlags |= FKF_AVAILABLE;

    // Set new state
    success = SystemParametersInfo(
        SPI_SETFILTERKEYS,
        sizeof(FILTERKEYS),
        &keys,
        SPIF_UPDATEINIFILE | SPIF_SENDCHANGE // Persist changes (write to registry)
    );

    if (!success)
        return ErrorExit("Failed to set new FilterKeys struct");

    system("pause");
}
