// Screen.h
// CSOPESY MO3 Group 5
//
// Keeps track of shared terminal state. Two threads draw to this console.
// You need to hold the screen lock before writing to output.
// This stops threads from interrupting each other and leaving the cursor in weird spots.

#pragma once

#include <mutex>

namespace Screen {

// The marquee owns these top rows.
const int MARQUEE_ROWS = 5;

// The command area begins below the marquee.
const int COMMAND_TOP = MARQUEE_ROWS + 2;

// Gets the lock for terminal output.
std::mutex& lock();

// Sets up the terminal for our needs.
void init();

// Restores the original terminal settings.
void shutdown();

// Checks if a real user is typing.
bool interactive();

// Gets the terminal width.
int  width();

// Gets the terminal height.
int  height();

// Moves the cursor to a specific row and column.
void moveTo(int row, int col);

// Clears the current line.
void eraseLine();

// Restricts scrolling to a specific area.
void setScrollRegion(int top, int bottom);

// Lets the whole screen scroll normally again.
void resetScrollRegion();

// Reads a key press right away and returns 0 if nothing is pressed.
int readKey();

} // namespace Screen
