// Marquee.cpp
// CSOPESY MO3 Group 5
//
// The animation runs on a background thread.
// It draws the ASCII banner across rows 1 to 5 without disturbing user input.

#include "Marquee.h"

#include <iostream>
#include <chrono>

static const std::vector<std::string> DEFAULT_BANNER = {
    R"(  ____ ____   ___  ____  _____ ____  __   __)",
    R"( / ___/ ___| / _ \|  _ \| ____/ ___| \ \ / /)",
    R"(| |   \___ \| | | | |_) |  _| \___ \  \ V / )",
    R"(| |___ ___) | |_| |  __/| |___ ___) |  | |  )",
    R"( \____|____/ \___/|_|   |_____|____/   |_|  )"
};

static void moveTo(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
}

static void eraseLine() {
    std::cout << "\033[2K";
}

Marquee::Marquee()
    : m_lines(DEFAULT_BANNER),
      m_speedMs(100),
      m_running(false)
{}

Marquee::~Marquee() {
    // Stop thread if still running when deleted.
    if (m_running.load()) {
        m_running.store(false);
        if (m_thread.joinable())
            m_thread.join();
    }
}

bool Marquee::start() {
    if (m_running.load())
        return false;

    m_running.store(true);
    m_thread = std::thread(&Marquee::animationLoop, this);
    return true;
}

bool Marquee::stop() {
    if (!m_running.load())
        return false;

    m_running.store(false);
    if (m_thread.joinable())
        m_thread.join();

    drawHome();
    return true;
}

bool Marquee::isRunning() const {
    return m_running.load();
}

void Marquee::setText(const std::string& text) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (text.empty() || text == "default" || text == "csopesy") {
            m_lines = DEFAULT_BANNER;
        } else {
            m_lines = { text };
        }
    }
    if (!m_running.load()) {
        drawHome();
    }
}

void Marquee::setSpeedMs(int ms) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_speedMs = ms;
}

void Marquee::drawHome() const {
    std::vector<std::string> lines;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        lines = m_lines;
    }

    const int TOTAL_ROWS = 5;
    std::cout << "\033[s";
    for (int r = 0; r < TOTAL_ROWS; ++r) {
        moveTo(1 + r, 1);
        eraseLine();
        if (r < static_cast<int>(lines.size())) {
            std::cout << lines[r];
        }
    }
    std::cout << "\033[u";
    std::cout.flush();
}

void Marquee::animationLoop() {
    const int WINDOW_WIDTH = 80;
    const int TOTAL_ROWS = 5;
    int offset = 0;

    while (m_running.load()) {
        std::vector<std::string> lines;
        int speedMs;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            lines   = m_lines;
            speedMs = m_speedMs;
        }

        int maxLen = 0;
        for (const auto& l : lines) {
            if (static_cast<int>(l.size()) > maxLen) {
                maxLen = static_cast<int>(l.size());
            }
        }

        const int totalRange = WINDOW_WIDTH + maxLen;
        const int textStart = WINDOW_WIDTH - offset;

        // Save cursor, draw all banner rows, then restore cursor.
        std::cout << "\033[s";
        for (int r = 0; r < TOTAL_ROWS; ++r) {
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
                std::cout << line;
            }
        }
        std::cout << "\033[u";
        std::cout.flush();

        // Move banner one step left and wrap around at the end.
        offset = (offset + 1) % totalRange;

        std::this_thread::sleep_for(std::chrono::milliseconds(speedMs));
    }
}

