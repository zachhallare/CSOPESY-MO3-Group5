// Console.cpp
// CSOPESY MO3 Group 5

#include "Console.h"

#include <iostream>
#include <string>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

// Clear the screen and reset the cursor to the top left.
static void clearScreen() {
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

Console::Console() {}

void Console::printHeader() const {
    std::cout << R"(  ____ ____   ___  ____  _____ ____  __   __
 / ___/ ___| / _ \|  _ \| ____/ ___| \ \ / /
| |   \___ \| | | | |_) |  _| \___ \  \ V / 
| |___ ___) | |_| |  __/| |___ ___) |  | |  
 \____|____/ \___/|_|   |_____|____/   |_|  
)" << "\n";

    std::cout << "Hello, Welcome to CSOPESY OS Emulator!\n\n"
                 "Group developer:\n"
                 "    Hallare, Zach Benedict I.\nRamos, Margaret Patrice M.\nTan, Neil Justine J.\n\n"
                 "Version date: " __DATE__ "\n\n";
}

void Console::printHelp() const {
    std::cout << "\nAvailable commands:\n"
                 "  start_marquee  - Start the scrolling marquee animation\n"
                 "  stop_marquee   - Stop  the scrolling marquee animation\n"
                 "  set_text       - Set custom marquee text\n"
                 "  set_speed      - Set animation speed in milliseconds\n"
                 "  help           - Display this help message\n"
                 "  exit           - Exit the emulator\n\n";
}

void Console::handleStartMarquee() {
    if (m_marquee.start()) {
        std::cout << "Marquee started.\n";
    } else {
        std::cout << "Marquee is already running.\n";
    }
}

void Console::handleStopMarquee() {
    if (m_marquee.stop()) {
        std::cout << "Marquee stopped.\n";
    } else {
        std::cout << "Marquee is not running.\n";
    }
}

void Console::run() {
    // Turn on ANSI escape codes and UTF-8 for Windows 10 and up.
#ifdef _WIN32
    {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        SetConsoleOutputCP(CP_UTF8);
    }
#endif

    clearScreen();
    printHeader();

    std::string input;

    while (true) {
        std::cout << "Command> ";
        std::cout.flush();

        if (!std::getline(std::cin, input))
            break;

        // Trim whitespace from both ends.
        auto start = input.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            continue;
        }
        auto end = input.find_last_not_of(" \t\r\n");
        input = input.substr(start, end - start + 1);

        // Split command and argument.
        std::string cmd = input;
        std::string arg;
        auto spacePos = input.find(' ');
        if (spacePos != std::string::npos) {
            cmd = input.substr(0, spacePos);
            auto argStart = input.find_first_not_of(" \t", spacePos);
            if (argStart != std::string::npos) {
                arg = input.substr(argStart);
            }
        }
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if (cmd == "start_marquee") {
            handleStartMarquee();
        } else if (cmd == "stop_marquee") {
            handleStopMarquee();
        } else if (cmd == "set_text") {
            if (arg.empty()) {
                std::cout << "Usage: set_text <text>\n";
            } else {
                m_marquee.setText(arg);
                std::cout << "Marquee text updated.\n";
            }
        } else if (cmd == "set_speed") {
            if (arg.empty()) {
                std::cout << "Usage: set_speed <milliseconds>\n";
            } else {
                try {
                    int ms = std::stoi(arg);
                    if (ms <= 0) {
                        std::cout << "Speed must be a positive number.\n";
                    } else {
                        m_marquee.setSpeedMs(ms);
                        std::cout << "Marquee speed set to " << ms << " ms.\n";
                    }
                } catch (...) {
                    std::cout << "Invalid speed value.\n";
                }
            }
        } else if (cmd == "help") {
            printHelp();
        } else if (cmd == "exit") {
            // Stop the marquee before quitting.
            if (m_marquee.isRunning()) {
                m_marquee.stop();
            }
            std::cout << "Goodbye!\n";
            break;
        } else {
            std::cout << "Unknown command: \"" << input << "\". Type \"help\" for a list of commands.\n";
        }
    }
}

