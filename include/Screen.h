// Screen.h
// CSOPESY MO3 Group 5
//
// Shared terminal state. Two threads draw to this console: the main thread
// (prompt, command output) and the marquee thread. Every write to std::cout
// must be made while holding Screen::lock(), otherwise one thread cuts into
// the middle of the other's escape sequence and the cursor is left parked
// wherever that sequence happened to stop.

#pragma once

#include <mutex>

namespace Screen {

// Rows 1..MARQUEE_ROWS are owned by the marquee.
const int MARQUEE_ROWS = 5;

// The command area starts below the marquee, with one blank row between.
const int COMMAND_TOP = MARQUEE_ROWS + 2;

// Guards all writes to std::cout.
std::mutex& lock();

void init();      // enable ANSI + UTF-8, switch input to unbuffered/no-echo
void shutdown();  // put the console mode back the way we found it

bool interactive();  // false when stdin is a pipe or file
int  width();
int  height();

void moveTo(int row, int col);
void eraseLine();
void setScrollRegion(int top, int bottom);
void resetScrollRegion();

// Non-blocking key read. Returns 0 when nothing is waiting.
int readKey();

} // namespace Screen
