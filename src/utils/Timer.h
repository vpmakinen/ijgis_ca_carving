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

#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>

class Timer {
public:
    Timer();
    Timer(const Timer & other);
    virtual ~Timer();

    void start();
    void stop();
    size_t total_time() const;
    size_t average_time() const;
    bool running() const;
    size_t last_interval() const;

    std::chrono::time_point<std::chrono::high_resolution_clock> starting_time() const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_, stop_;
    size_t total_time_;
    size_t last_interval_;
    unsigned int n_measurements_;
    bool running_;
};

#endif /* TIMER_H_ */
