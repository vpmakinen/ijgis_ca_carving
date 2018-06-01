/**
 * Copyright
 *   2015-     Finnish Geospatial Research Institute,
 *             National Land Survey of Finland
 *
 * Programmers: Ville Mäkinen
 *
 * This file is released under the GNU Lesser General Public
 * Licence version 2.1.
 */

#include "Timer.h"
#include <chrono>
#include "logging.h"

Timer::Timer() :
    total_time_ {0},
    n_measurements_ {0},
    running_ {false}
{
}

Timer::Timer(const Timer & other) :
    start_(other.start_),
    stop_(other.stop_),
    total_time_(other.total_time_),
    last_interval_(other.last_interval_),
    n_measurements_(other.n_measurements_),
    running_(other.running_)
{
}

Timer::~Timer()
{
    if (running_) {
        stop();
    }
}

void Timer::start() {
    if (running_) {
        logging::pErr() << "Timer is already running!\n";
        return;
    }
    else {
        running_ = true;
    }

    start_ = std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
    if (!running_) {
        logging::pErr() << "Timer has already stopped!\n";
        return;
    }
    stop_ = std::chrono::high_resolution_clock::now();
    size_t elapsed {static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(stop_ - start_).count())};
    total_time_ += elapsed;
    last_interval_ = elapsed;
    ++n_measurements_;
    running_ = false;
}

size_t Timer::total_time() const
{
    return total_time_;
}

size_t Timer::last_interval() const
{
    return last_interval_;
}

size_t Timer::average_time() const {
    if (n_measurements_) {
        return total_time_ / n_measurements_;
    } else {
        return 0;
    }
}

bool Timer::running() const
{
    return running_;
}

std::chrono::time_point<std::chrono::high_resolution_clock> Timer::starting_time() const
{
    return start_;
}
