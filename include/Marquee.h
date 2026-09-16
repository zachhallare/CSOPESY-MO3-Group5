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
    Marquee();
    ~Marquee();

    // Start the marquee on a background thread.
    // Return false if it is already running.
    bool start();

    // Stop the background thread and wait for it to finish.
    // Return false if it is not running.
    bool stop();

    bool isRunning() const;

    void setText(const std::string& text);
    void setSpeedMs(int ms);
    void drawHome() const;

private:
    void animationLoop();

    std::vector<std::string> m_lines;
    int                      m_speedMs;
    std::atomic<bool>        m_running;
    std::thread              m_thread;
    mutable std::mutex       m_mutex; // lock for m_lines and m_speedMs
};

