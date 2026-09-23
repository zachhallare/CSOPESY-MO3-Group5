// Console.h
// CSOPESY MO3 Group 5

#pragma once

#include "Marquee.h"
#include <string>

class Console {
public:
    Console();

    // Run the command loop until the user types exit.
    void run();

private:
    // Display helpers
    void printHeader() const;
    void printHelp()   const;

    // Command handlers
    void handleStartMarquee();
    void handleStopMarquee();
    void handleSetText(const std::string& arg);
    void handleSetSpeed(const std::string& arg);

    void printPrompt() const;

    // Runs one line of input. Returns false when the user asked to exit.
    bool executeCommand(const std::string& raw);

    Marquee m_marquee;
};

