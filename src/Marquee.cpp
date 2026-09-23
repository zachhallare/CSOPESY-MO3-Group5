// Marquee.cpp
// CSOPESY MO3 Group 5
//
// The animation runs on a background thread.
// It draws the ASCII banner across rows 1 to 5 without disturbing user input.

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

static void moveTo(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
}

static void eraseLine() {
    std::cout << "\033[2K";
}

Marquee::Marquee()
    : m_lines(DEFAULT_BANNER),
      m_speedMs(100),
      m_offset(0),
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

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_offset = 0;
    }

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
        if (text.empty()) {
            // Only a blank input falls back to the banner. Any other input is
            // shown literally, so "set_text default" really does print
            // "default".
            m_lines = DEFAULT_BANNER;
        } else {
            m_lines = { text };
        }
        // Restart the scroll so the new text enters from the right edge
        // instead of appearing mid-slide at the old offset.
        m_offset = 0;
    }
    if (!m_running.load()) {
        drawHome();
    }
}

void Marquee::setSpeedMs(int ms) {
    if (ms < MIN_SPEED_MS) ms = MIN_SPEED_MS;
    if (ms > MAX_SPEED_MS) ms = MAX_SPEED_MS;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_speedMs = ms;
}

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

        // Follow the real window size. At a hardcoded 80 a narrower console
        // wraps every banner row onto the line below it, which walks the
        // marquee down into the command area.
        const int WINDOW_WIDTH = Screen::width();

        int maxLen = 0;
        for (const auto& l : lines) {
            if (static_cast<int>(l.size()) > maxLen) {
                maxLen = static_cast<int>(l.size());
            }
        }

        const int totalRange = WINDOW_WIDTH + maxLen;
        const int textStart = WINDOW_WIDTH - offset;

        // Save cursor, draw all banner rows, then restore cursor. The whole
        // frame is one critical section so the main thread cannot print
        // between the save and the restore.
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

                    // Trim the trailing blanks. eraseLine already cleared the
                    // row, and writing the final column would wrap the cursor
                    // onto the next line.
                    const std::size_t lastCh = line.find_last_not_of(' ');
                    if (lastCh != std::string::npos) {
                        std::cout << line.substr(0, lastCh + 1);
                    }
                }
            }
            std::cout << "\033[u";
            std::cout.flush();
        }

        // Move banner one step left and wrap around at the end. If set_text
        // reset the offset while this frame was drawing, leave it at 0 so the
        // new text still starts from the right edge.
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_offset == offset) {
                m_offset = (offset + 1) % totalRange;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(speedMs));
    }
}

