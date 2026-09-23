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
    // Allowed speed limits in milliseconds.
    static const int MIN_SPEED_MS = 1;
    static const int MAX_SPEED_MS = 60000;

    // Sets up the marquee.
    Marquee();

    // Cleans up the background thread.
    ~Marquee();

    // Starts the animation in the background and returns false if it is already going.
    bool start();

    // Stops the animation and waits for it to finish. Returns false if it was not running.
    bool stop();

    // Checks if the animation is currently active.
    bool isRunning() const;

    // Updates the text. An empty string brings back the default banner.
    void setText(const std::string& text);

    // Changes the animation speed and keeps it within the allowed limits.
    void setSpeedMs(int ms);

    // Draws the text at the starting position.
    void drawHome() const;

private:
    // Runs the animation loop in the background.
    void animationLoop();

    std::vector<std::string> m_lines;
    int                      m_speedMs;
    int                      m_offset;  // Tracks the scroll position.
    std::atomic<bool>        m_running;
    std::thread              m_thread;
    mutable std::mutex       m_mutex; // Keeps our shared data safe from other threads.
};

