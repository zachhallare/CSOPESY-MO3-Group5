// Console.cpp
// CSOPESY MO3 Group 5

#include "Console.h"
#include "Screen.h"

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <thread>
#include <chrono>

// How often the input loop checks the keyboard.
static const int POLL_MS = 10;

// Prints text safely by holding the output lock.
static void say(const std::string& text) {
    std::lock_guard<std::mutex> guard(Screen::lock());
    std::cout << text;
    std::cout.flush();
}

// Removes one matching pair of surrounding quotes.
static std::string stripQuotes(const std::string& s) {
    if (s.size() >= 2) {
        const char f = s.front();
        const char b = s.back();
        if ((f == '"' && b == '"') || (f == '\'' && b == '\'')) {
            return s.substr(1, s.size() - 2);
        }
    }
    return s;
}

// Sets up the console.
Console::Console() {}

// Prints the header text.
void Console::printHeader() const {
    // Only print text below the marquee banner.
    m_marquee.drawHome();

    std::ostringstream out;
    out << "Hello, Welcome to CSOPESY OS Emulator!\n\n"
           "Group developer:\n"
           "    Hallare, Zach Benedict I.\n"
           "    Ramos, Margaret Patrice M.\n"
           "    Tan, Neil Justine J.\n\n"
           "Version date: " __DATE__ "\n\n";

    std::lock_guard<std::mutex> guard(Screen::lock());
    Screen::moveTo(Screen::COMMAND_TOP, 1);
    std::cout << out.str();
    std::cout.flush();
}

// Prints the help menu.
void Console::printHelp() const {
    say("\nAvailable commands:\n"
        "  start_marquee     - Start the scrolling marquee animation\n"
        "  stop_marquee      - Stop  the scrolling marquee animation\n"
        "  set_text <text>   - Display the given text as the marquee\n"
        "  set_speed <ms>    - Set the marquee refresh rate in milliseconds\n"
        "  help              - Display this help message\n"
        "  exit              - Exit the emulator\n\n");
}

// Starts the marquee animation.
void Console::handleStartMarquee() {
    if (m_marquee.start()) {
        say("Marquee started.\n");
    } else {
        say("Marquee is already running.\n");
    }
}

// Stops the marquee animation.
void Console::handleStopMarquee() {
    if (m_marquee.stop()) {
        say("Marquee stopped.\n");
    } else {
        say("Marquee is not running.\n");
    }
}

// Sets the marquee text.
void Console::handleSetText(const std::string& arg) {
    const std::string text = stripQuotes(arg);

    if (text.empty()) {
        say("Usage: set_text <text>\n");
        return;
    }

    m_marquee.setText(text);
    say("Marquee text set to: " + text + "\n");
}

// Sets the marquee speed.
void Console::handleSetSpeed(const std::string& arg) {
    const std::string value = stripQuotes(arg);

    if (value.empty()) {
        say("Usage: set_speed <milliseconds>\n");
        return;
    }

    // Make sure the value only contains numbers.
    if (value.find_first_not_of("0123456789") != std::string::npos) {
        say("Invalid speed: \"" + value + "\" is not a positive whole number.\n");
        return;
    }

    long ms = 0;
    try {
        ms = std::stol(value);
    } catch (...) {
        ms = -1;   // out of range for a long
    }

    if (ms < Marquee::MIN_SPEED_MS || ms > Marquee::MAX_SPEED_MS) {
        std::ostringstream msg;
        msg << "Speed must be between " << Marquee::MIN_SPEED_MS
            << " and " << Marquee::MAX_SPEED_MS << " ms.\n";
        say(msg.str());
        return;
    }

    m_marquee.setSpeedMs(static_cast<int>(ms));

    std::ostringstream msg;
    msg << "Marquee refresh rate set to " << ms << " ms.\n";
    say(msg.str());
}

// Prints the command prompt.
void Console::printPrompt() const {
    say("Command> ");
}

// Runs a single command.
bool Console::executeCommand(const std::string& raw) {
    // Trim whitespace.
    auto start = raw.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return true;
    }
    auto end = raw.find_last_not_of(" \t\r\n");
    const std::string input = raw.substr(start, end - start + 1);

    // Split the command and argument.
    std::string cmd = input;
    std::string arg;
    auto spacePos = input.find_first_of(" \t");
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
        handleSetText(arg);
    } else if (cmd == "set_speed") {
        handleSetSpeed(arg);
    } else if (cmd == "help") {
        printHelp();
    } else if (cmd == "exit") {
        // Stop the marquee before quitting.
        if (m_marquee.isRunning()) {
            m_marquee.stop();
        }
        say("Goodbye!\n");
        return false;
    } else {
        say("Unknown command: \"" + input + "\". Type \"help\" for a list of commands.\n");
    }
    return true;
}

// Runs the main command loop.
void Console::run() {
    Screen::init();

    {
        std::lock_guard<std::mutex> guard(Screen::lock());
        std::cout << "\033[2J\033[H";

        // Keep scrolling within the command area only.
        Screen::setScrollRegion(Screen::COMMAND_TOP, Screen::height());
        std::cout.flush();
    }

    printHeader();
    printPrompt();

    std::string buffer;
    bool running = true;

    if (!Screen::interactive()) {
        // Read lines directly when not interactive.
        std::string line;
        while (running && std::getline(std::cin, line)) {
            say(line + "\n");
            running = executeCommand(line);
            if (running) {
                printPrompt();
            }
        }
    } else {
        while (running) {
            const int key = Screen::readKey();

            if (key == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(POLL_MS));
                continue;
            }

            if (key == '\r' || key == '\n') {
                say("\n");
                running = executeCommand(buffer);
                buffer.clear();
                if (running) {
                    printPrompt();
                }
            } else if (key == '\b' || key == 127) {
                if (!buffer.empty()) {
                    buffer.pop_back();
                    say("\b \b");
                }
            } else if (key >= 32 && key < 127) {
                buffer.push_back(static_cast<char>(key));
                say(std::string(1, static_cast<char>(key)));
            }
            // Ignore other control keys.
        }
    }

    // Reset the terminal scroll region before exiting.
    {
        std::lock_guard<std::mutex> guard(Screen::lock());
        Screen::resetScrollRegion();
        Screen::moveTo(Screen::height(), 1);
        std::cout << "\n";
        std::cout.flush();
    }

    Screen::shutdown();
}
