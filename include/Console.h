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

    Marquee m_marquee;
};

