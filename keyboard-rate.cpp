#define NOMINMAX

#include <iostream>
#include <Windows.h>
#include <cstdlib>

// Alternative to system("pause") which caused AV false positives
void WaitForAnyKey()
{
    std::cout << "Press Enter to continue...";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int ErrorExit(const std::string& message)
{
    std::cerr << message << " (error code: " << GetLastError() << ")" << std::endl;
    WaitForAnyKey();
    std::exit(EXIT_FAILURE);
}

void PrintState(const FILTERKEYS& keys)
{
    std::cout << "\tAcceptance Delay: " << keys.iWaitMSec << std::endl;
    std::cout << "\tDelay Until Repeat: " << keys.iDelayMSec << std::endl;
    std::cout << "\tRepeat Rate: " << keys.iRepeatMSec << std::endl;
    std::cout << "\tDebounce Time: " << keys.iBounceMSec << std::endl;
    std::cout << "\tFlags: " << keys.dwFlags << std::endl;
}

void ParseInput(const std::string& message, DWORD& dest)
{
    std::cout << message;
    std::cin >> dest;
    if (std::cin.fail())
        ErrorExit("Invalid input");
}

void SetState(const FILTERKEYS& keys, bool persist)
{
    DWORD flags = SPIF_SENDCHANGE;
    if (persist)
        flags |= SPIF_UPDATEINIFILE; // write to registry to persist changes

    BOOL success = SystemParametersInfo(
        SPI_SETFILTERKEYS,
        sizeof(FILTERKEYS),
        (PVOID)&keys,
        flags
    );
    
    if (!success)
        ErrorExit("Failed to set new FilterKeys struct");
}

int main(int argc, char* argv[])
{
    BOOL success;

    // Get current FilterKeys struct
    FILTERKEYS original_keys;
    ZeroMemory(&original_keys, sizeof(FILTERKEYS));
    original_keys.cbSize = sizeof(FILTERKEYS);
    success = SystemParametersInfo(SPI_GETFILTERKEYS, sizeof(FILTERKEYS), &original_keys, 0);         
    if (!success)
        return ErrorExit("Failed to get current FilterKeys struct");

    std::cout << "Current State: " << std::endl;
    PrintState(original_keys);
    std::cout << std::endl;

    // New state
    FILTERKEYS new_keys = original_keys;
    ParseInput("Enter new acceptance delay: ", new_keys.iWaitMSec);
    ParseInput("Enter new delay until repeat: ", new_keys.iDelayMSec);
    ParseInput("Enter new repeat rate: ", new_keys.iRepeatMSec);
    ParseInput("Enter new debounce time: ", new_keys.iBounceMSec);

    // Necessary flags
    new_keys.dwFlags |= FKF_FILTERKEYSON;
    new_keys.dwFlags |= FKF_AVAILABLE;

    // Set new state (temporary)
    std::cout << "\nSetting new state:" << std::endl;
    PrintState(new_keys);
    SetState(new_keys, false);

    // Keep or revert changes
    std::cout << std::endl;
    std::cout << "Changes temporarily applied, if you cannot type you can logout/restart to revert the changes" << std::endl;
    std::cout << "Keep changes? (y/n): ";
    char confirm = 'n';
    std::cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        SetState(original_keys, false);
        std::cout << "Restored original state." << std::endl;
    }
    else {
        SetState(new_keys, true);
        std::cout << "Done" << std::endl;
    }

    WaitForAnyKey();
}
