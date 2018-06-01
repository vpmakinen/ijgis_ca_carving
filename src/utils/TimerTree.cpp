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

#include "TimerTree.h"

#include <sstream>
#include <iostream>
#include <iomanip>

#include "logging.h"
#include "Timer.h"

TimerTree::TimerTree()
{
    std::shared_ptr<Timer> t = std::make_shared<Timer>();
    setTimer(t);
    setDefaultValues();
}

TimerTree::TimerTree(const TimerTree & other) :
        descr_(other.descr_),
        indentation_(other.indentation_),
        widthTime_(other.widthTime_),
        widthPercent_(other.widthPercent_),
        timer_(other.timer_),
        children_(other.children_),
        root_timer_(other.root_timer_) {}

TimerTree::TimerTree(const std::string & descr, std::shared_ptr<Timer> const& timer)
{
    setDefaultValues();
    setTimer(timer);
    setDescr(descr);
}

TimerTree::TimerTree(const std::string & descr, bool root_timer)
{
    root_timer_ = root_timer;
    setDefaultValues();
    setTimer(std::make_shared<Timer>());
    setDescr(descr);
}

TimerTree::~TimerTree() {}

void TimerTree::setDefaultValues() {
    indentation_ = 2;
    widthTime_ = 11;
    widthPercent_ = 12;
    descr_ = std::string("Unnamed timer");
}

std::string TimerTree::descr() const
{
    return descr_;
}


std::shared_ptr<Timer> TimerTree::getTimer()
{
    return this->timer_;
}

void TimerTree::setTimer(std::shared_ptr<Timer> const& timer)
{
    this->timer_ = timer;
}

void TimerTree::setDescr(std::string name)
{
    this->descr_ = std::string(name);
}

void TimerTree::start()
{
    this->timer_->start();
}

void TimerTree::stop()
{
    this->timer_->stop();
}

void TimerTree::addChild(std::shared_ptr<TimerTree> const& timerTree)
{
    children_.push_back(timerTree);
}

void TimerTree::addChild(const std::string & name, std::shared_ptr<Timer> const& timer)
{
    std::shared_ptr<TimerTree> p = std::make_shared<TimerTree>(name, timer);
    children_.push_back(p);
}

std::shared_ptr<TimerTree> TimerTree::addChild(const std::string &name)
{
    std::shared_ptr<TimerTree> c = std::make_shared<TimerTree>(name);
    addChild(c);
    return c;
}

/*
std::shared_ptr<TimerTree> TimerTree::findTimerWithDescr(const std::string &name) const
{
    if (this->descr_ == name) {
        return std::make_shared<TimerTree>(*this);
    }
    std::shared_ptr<TimerTree> ret = nullptr;
    for (auto it = children_.begin(); it != children_.end(); ++it) {
         ret = (*it)->findTimerWithDescr(name);
         if (ret != nullptr) {
             break;
         }
    }
    return ret;
}
*/

bool TimerTree::hasChildren() const
{
    return children_.size();
}

unsigned int TimerTree::getMaxNameWidth() const
{
    if (timer_->total_time() == 0) {
        return 0;
    }
    unsigned int max = static_cast<unsigned int>(descr_.size());
    for (auto it = children_.begin(); it != children_.end(); ++it) {
        std::shared_ptr<TimerTree> t = *it;
        auto tmax = t->getMaxNameWidth();
        if (tmax > max) {
            max = tmax;
        }
    }
    return max;
}

int TimerTree::getMaxDepth() const
{
    int max = 0;
    if (timer_->total_time() == 0) {
        return max;
    }
    if (!hasChildren()) {
        return max;
    }
    for (auto it = children_.begin(); it != children_.end(); ++it) {
        std::shared_ptr<TimerTree> t = *it;
        int tmax = t->getMaxDepth();
        if (tmax > max) {
            max = tmax;
        }
    }
    return max + 1;
}

/* 
 * +---------------+--------------------------+----------------+-------------+
 * | Timer summary |               Total time |  Average time  |  % of total |
 * +---------------+------+-------------+-----+----------+-----+-------------+
 * | timer1               |        12.4 |  ms |      0.6 |  ms |        76.4 |
 * +---+------------------+-------------+-----+----------+-----+-------------+
 * |   | subtimer1        |         8.4 |  ms |      0.4 |  ms |         4.5 |
 * +---+---+--------------+-------------+-----+----------+-----+-------------+
 * |       | subsubtimer1 |         8.4 |  ms |      0.4 |  ms |         4.5 |
 * +-------+--------------+-------------+-----+----------+-----+-------------+
 */
void TimerTree::summary(int level, size_t total, int nameWidth, int depth) const
{
    if (timer_->total_time() == 0) {
        return;
    }
    std::stringstream ss;
    ss << std::setw(indentation_ * level) << "";
    ss << std::setw(nameWidth + (depth - level) * indentation_) << std::left << descr_;
    ss << std::right << std::fixed << std::setprecision(1);
    ss << std::setw(widthTime_) << timer_->total_time() << " ms";
    ss << std::setw(widthTime_) << timer_->average_time() << " ms";
    ss << std::setw(widthPercent_) <<
        (100 * static_cast<double>(timer_->total_time()) /
            static_cast<double>(total));
    logging::pLog() << ss.str();
    for (auto it = children_.begin(); it != children_.end(); ++it) {
        (*it)->summary(level + 1, timer_->total_time(), nameWidth, depth);
    }
}

void TimerTree::summary() const
{
    size_t total {timer_->total_time()};
    int depth = getMaxDepth();
    auto nameWidth = static_cast<int>(getMaxNameWidth());
    int B = indentation_ * depth + nameWidth;
    std::stringstream ss;

    // Print the header line
    int w1 {std::max(12, B + widthTime_ + 3 - 13)};
    ss << std::setfill(' ') << std::right << "Timer summary";
    ss << std::setw(w1) << "Total time";
    ss << std::setw(widthTime_ + 3) << "Average time";
    ss << std::setw(widthPercent_) << "% of total";
    logging::pLog() << ss.str();

    summary(0, total, nameWidth, depth);
}


TimerController::TimerController(std::shared_ptr<TimerTree> const& timertree)
{
    timer_ = timertree->getTimer();
    start();
}

TimerController::~TimerController()
{
    if (timer_->running()) {
        stop();
    }
    timer_ = nullptr;
}

void TimerController::start()
{
    if (timer_->running()) {
        throw std::runtime_error("Timer is already running.");
    }
    timer_->start();
}

void TimerController::stop()
{
    if (!timer_->running()) {
        throw std::runtime_error("Timer is not running.");
    }
    timer_->stop();
}
