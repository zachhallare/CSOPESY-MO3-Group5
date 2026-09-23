// Console.h
// CSOPESY MO3 Group 5

#pragma once

#include "Marquee.h"
#include <string>

class Console {
public:
    // Sets up the console.
    Console();

    // Runs the command loop until the user exits.
    void run();

private:
    // Prints the header text.
    void printHeader() const;
    
    // Prints the available commands.
    void printHelp() const;

    // Starts the marquee animation.
    void handleStartMarquee();
    
    // Stops the marquee animation.
    void handleStopMarquee();
    
    // Sets the text for the marquee.
    void handleSetText(const std::string& arg);
    
    // Sets the speed of the marquee.
    void handleSetSpeed(const std::string& arg);

    // Prints the command prompt.
    void printPrompt() const;

    // Runs one line of input and returns false when it is time to exit.
    bool executeCommand(const std::string& raw);

    Marquee m_marquee;
};

