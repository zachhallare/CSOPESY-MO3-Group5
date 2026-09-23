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

// How often the input loop checks the keyboard. Low enough that typing feels
// immediate, high enough that the loop is not a busy-wait competing with the
// marquee thread for the output lock.
static const int POLL_MS = 10;

// Every write to std::cout goes through here. Holding Screen::lock() keeps
// this output from landing in the middle of a marquee frame.
static void say(const std::string& text) {
    std::lock_guard<std::mutex> guard(Screen::lock());
    std::cout << text;
    std::cout.flush();
}

// Remove one matching pair of surrounding quotes, so both
// set_text hello world  and  set_text "hello world"  give the same result.
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

Console::Console() {}

void Console::printHeader() const {
    // Rows 1..MARQUEE_ROWS are the marquee's; it paints the CSOPESY banner
    // there itself, so the header only prints the text below that band.
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

void Console::printHelp() const {
    say("\nAvailable commands:\n"
        "  start_marquee     - Start the scrolling marquee animation\n"
        "  stop_marquee      - Stop  the scrolling marquee animation\n"
        "  set_text <text>   - Display the given text as the marquee\n"
        "  set_speed <ms>    - Set the marquee refresh rate in milliseconds\n"
        "  help              - Display this help message\n"
        "  exit              - Exit the emulator\n\n");
}

void Console::handleStartMarquee() {
    if (m_marquee.start()) {
        say("Marquee started.\n");
    } else {
        say("Marquee is already running.\n");
    }
}

void Console::handleStopMarquee() {
    if (m_marquee.stop()) {
        say("Marquee stopped.\n");
    } else {
        say("Marquee is not running.\n");
    }
}

void Console::handleSetText(const std::string& arg) {
    const std::string text = stripQuotes(arg);

    if (text.empty()) {
        say("Usage: set_text <text>\n");
        return;
    }

    m_marquee.setText(text);
    say("Marquee text set to: " + text + "\n");
}

void Console::handleSetSpeed(const std::string& arg) {
    const std::string value = stripQuotes(arg);

    if (value.empty()) {
        say("Usage: set_speed <milliseconds>\n");
        return;
    }

    // The whole argument must be digits. std::stoi alone would silently
    // accept "100abc" as 100.
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

void Console::printPrompt() const {
    say("Command> ");
}

bool Console::executeCommand(const std::string& raw) {
    // Trim whitespace from both ends.
    auto start = raw.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return true;
    }
    auto end = raw.find_last_not_of(" \t\r\n");
    const std::string input = raw.substr(start, end - start + 1);

    // Split command and argument.
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

void Console::run() {
    Screen::init();

    {
        std::lock_guard<std::mutex> guard(Screen::lock());
        std::cout << "\033[2J\033[H";

        // Confine scrolling to the rows below the marquee. Without this the
        // console scrolls the whole window once output fills it, dragging the
        // prompt up into the rows the marquee repaints every frame -- which is
        // what makes "Command>" disappear.
        Screen::setScrollRegion(Screen::COMMAND_TOP, Screen::height());
        std::cout.flush();
    }

    printHeader();
    printPrompt();

    std::string buffer;
    bool running = true;

    if (!Screen::interactive()) {
        // stdin is a pipe or a file. Keyboard polling only works on a real
        // console, so fall back to blocking line reads and echo the line
        // ourselves, since nothing else will.
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
            // Anything else (control keys, escapes) is ignored.
        }
    }

    // Hand the terminal back the way we borrowed it. Leaving the scroll
    // region set would break the user's shell after we exit.
    {
        std::lock_guard<std::mutex> guard(Screen::lock());
        Screen::resetScrollRegion();
        Screen::moveTo(Screen::height(), 1);
        std::cout << "\n";
        std::cout.flush();
    }

    Screen::shutdown();
}
