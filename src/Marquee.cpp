// Marquee.cpp
// CSOPESY MO3 Group 5
//
// Runs the animation on a background thread.
// It draws the banner across the top rows.

#include "Marquee.h"
#include "Screen.h"

#include <iostream>
#include <chrono>

static const std::vector<std::string> DEFAULT_BANNER = {
    R"(  ____ ____   ___  ____  _____ ____  __   __)",
    R"( / ___/ ___| / _ \|  _ \| ____/ ___| \ \ / /)",
    R"(| |   \___ \| | | | |_) |  _| \___ \  \ V / )",
    R"(| |___ ___) | |_| |  __/| |___ ___) |  | |  )",
    R"( \____|____/ \___/|_|   |_____|____/   |_|  )"
};

// Moves the cursor to a specific spot.
static void moveTo(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
}

// Clears the current line.
static void eraseLine() {
    std::cout << "\033[2K";
}

// Initializes the marquee with default settings.
Marquee::Marquee()
    : m_lines(DEFAULT_BANNER),
      m_speedMs(100),
      m_offset(0),
      m_running(false)
{}

// Cleans up the thread if it is still running.
Marquee::~Marquee() {
    // Stop thread if still running when deleted.
    if (m_running.load()) {
        m_running.store(false);
        if (m_thread.joinable())
            m_thread.join();
    }
}

// Starts the background thread.
bool Marquee::start() {
    if (m_running.load())
        return false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_offset = 0;
    }

    m_running.store(true);
    m_thread = std::thread(&Marquee::animationLoop, this);
    return true;
}

// Stops the background thread.
bool Marquee::stop() {
    if (!m_running.load())
        return false;

    m_running.store(false);
    if (m_thread.joinable())
        m_thread.join();

    drawHome();
    return true;
}

// Checks if the animation is running.
bool Marquee::isRunning() const {
    return m_running.load();
}

// Replaces the marquee text.
void Marquee::setText(const std::string& text) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (text.empty()) {
            // Restore the default banner.
            m_lines = DEFAULT_BANNER;
        } else {
            m_lines = { text };
        }
        // Restart the scroll to enter from the right edge.
        m_offset = 0;
    }
    if (!m_running.load()) {
        drawHome();
    }
}

// Sets the animation speed in milliseconds.
void Marquee::setSpeedMs(int ms) {
    if (ms < MIN_SPEED_MS) ms = MIN_SPEED_MS;
    if (ms > MAX_SPEED_MS) ms = MAX_SPEED_MS;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_speedMs = ms;
}

// Draws the text at the home position.
void Marquee::drawHome() const {
    std::vector<std::string> lines;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        lines = m_lines;
    }

    std::lock_guard<std::mutex> guard(Screen::lock());
    std::cout << "\033[s";
    for (int r = 0; r < Screen::MARQUEE_ROWS; ++r) {
        moveTo(1 + r, 1);
        eraseLine();
        if (r < static_cast<int>(lines.size())) {
            std::cout << lines[r];
        }
    }
    std::cout << "\033[u";
    std::cout.flush();
}

// Runs the animation loop.
void Marquee::animationLoop() {
    while (m_running.load()) {
        std::vector<std::string> lines;
        int speedMs;
        int offset;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            lines   = m_lines;
            speedMs = m_speedMs;
            offset  = m_offset;
        }

        // Follow the real window size.
        const int WINDOW_WIDTH = Screen::width();

        int maxLen = 0;
        for (const auto& l : lines) {
            if (static_cast<int>(l.size()) > maxLen) {
                maxLen = static_cast<int>(l.size());
            }
        }

        const int totalRange = WINDOW_WIDTH + maxLen;
        const int textStart = WINDOW_WIDTH - offset;

        // Draw the frame while holding the lock.
        {
            std::lock_guard<std::mutex> guard(Screen::lock());
            std::cout << "\033[s";
            for (int r = 0; r < Screen::MARQUEE_ROWS; ++r) {
                moveTo(1 + r, 1);
                eraseLine();

                if (r < static_cast<int>(lines.size())) {
                    const std::string& src = lines[r];
                    const int len = static_cast<int>(src.size());
                    std::string line(WINDOW_WIDTH, ' ');

                    for (int i = 0; i < len; ++i) {
                        int col = textStart + i;
                        if (col >= 0 && col < WINDOW_WIDTH) {
                            line[col] = src[i];
                        }
                    }

                    // Trim trailing blanks to prevent wrapping.
                    const std::size_t lastCh = line.find_last_not_of(' ');
                    if (lastCh != std::string::npos) {
                        std::cout << line.substr(0, lastCh + 1);
                    }
                }
            }
            std::cout << "\033[u";
            std::cout.flush();
        }

        // Move the banner left and wrap around.
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_offset == offset) {
                m_offset = (offset + 1) % totalRange;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(speedMs));
    }
}

