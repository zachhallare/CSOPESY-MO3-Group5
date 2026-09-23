// Screen.cpp
// CSOPESY MO3 Group 5

#include "Screen.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <io.h>
// Older headers might not define this but the console still uses it on newer Windows.
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

namespace Screen {

// Gets the console output lock.
std::mutex& lock() {
    static std::mutex m;
    return m;
}

#ifdef _WIN32

static DWORD g_savedInputMode = 0;
static bool  g_inputModeSaved = false;

// Handles closing the program to restore terminal settings.
static BOOL WINAPI ctrlHandler(DWORD type);

// Sets up the terminal for our needs.
void init() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD outMode = 0;
    if (GetConsoleMode(hOut, &outMode)) {
        SetConsoleMode(hOut, outMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    SetConsoleOutputCP(CP_UTF8);

    // Turn off line buffering and echo.
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (GetConsoleMode(hIn, &g_savedInputMode)) {
        g_inputModeSaved = true;
        SetConsoleMode(hIn, g_savedInputMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
    }

    // Set up the custom control handler to restore terminal settings on exit.
    SetConsoleCtrlHandler(ctrlHandler, TRUE);
}

// Restores the original terminal settings.
void shutdown() {
    if (g_inputModeSaved) {
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), g_savedInputMode);
    }
}

static BOOL WINAPI ctrlHandler(DWORD type) {
    if (type == CTRL_C_EVENT || type == CTRL_BREAK_EVENT || type == CTRL_CLOSE_EVENT) {
        shutdown();
        std::cout << "\033[r" << std::endl;   // Release the scroll region.
    }
    return FALSE;   // Let the default handler close the program.
}

// Checks if a real user is typing.
bool interactive() {
    return _isatty(_fileno(stdin)) != 0;
}

// Gets terminal buffer information.
static bool bufferInfo(CONSOLE_SCREEN_BUFFER_INFO& info) {
    return GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info) != 0;
}

// Gets the terminal width.
int width() {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (bufferInfo(info)) {
        return info.srWindow.Right - info.srWindow.Left + 1;
    }
    return 80;
}

// Gets the terminal height.
int height() {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (bufferInfo(info)) {
        return info.srWindow.Bottom - info.srWindow.Top + 1;
    }
    return 25;
}

// Reads a key press right away.
int readKey() {
    if (!_kbhit()) {
        return 0;
    }
    int c = _getch();
    if (c == 0 || c == 224) {
        _getch();   // Ignore the second byte of special keys.
        return 0;
    }
    return c;
}

#else

static struct termios g_savedTerm;
static bool           g_termSaved = false;

// Sets up the terminal for our needs.
void init() {
    struct termios t;
    if (tcgetattr(STDIN_FILENO, &t) == 0) {
        g_savedTerm = t;
        g_termSaved = true;
        t.c_lflag &= ~(ICANON | ECHO);
        t.c_cc[VMIN]  = 0;   // Set to non-blocking read.
        t.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }
}

// Restores the original terminal settings.
void shutdown() {
    if (g_termSaved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_savedTerm);
    }
}

// Checks if a real user is typing.
bool interactive() {
    return isatty(STDIN_FILENO) != 0;
}

// Gets the terminal width.
int width() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        return ws.ws_col;
    }
    return 80;
}

// Gets the terminal height.
int height() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 0) {
        return ws.ws_row;
    }
    return 25;
}

// Reads a key press right away.
int readKey() {
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        return c;
    }
    return 0;
}

#endif

// Moves the cursor to a specific row and column.
void moveTo(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
}

// Clears the current line.
void eraseLine() {
    std::cout << "\033[2K";
}

// Restricts scrolling to a specific area.
void setScrollRegion(int top, int bottom) {
    std::cout << "\033[" << top << ";" << bottom << "r";
}

// Lets the whole screen scroll normally again.
void resetScrollRegion() {
    std::cout << "\033[r";
}

} // namespace Screen
