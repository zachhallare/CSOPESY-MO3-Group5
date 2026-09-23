// Marquee.h
// CSOPESY MO3 Group 5

#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

class Marquee {
public:
    // Accepted range for set_speed, in milliseconds.
    static const int MIN_SPEED_MS = 1;
    static const int MAX_SPEED_MS = 60000;

    Marquee();
    ~Marquee();

    // Start the marquee on a background thread.
    // Return false if it is already running.
    bool start();

    // Stop the background thread and wait for it to finish.
    // Return false if it is not running.
    bool stop();

    bool isRunning() const;

    // Replace the marquee text. An empty string restores the CSOPESY banner.
    void setText(const std::string& text);

    // Set the animation refresh rate. The value is clamped to
    // [MIN_SPEED_MS, MAX_SPEED_MS].
    void setSpeedMs(int ms);

    void drawHome() const;

private:
    void animationLoop();

    std::vector<std::string> m_lines;
    int                      m_speedMs;
    int                      m_offset;  // current scroll position
    std::atomic<bool>        m_running;
    std::thread              m_thread;
    mutable std::mutex       m_mutex; // lock for m_lines, m_speedMs and m_offset
};

