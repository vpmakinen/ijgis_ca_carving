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

#ifndef TIMERTREE_H_
#define TIMERTREE_H_

#include <string>
#include <vector>
#include <memory> // std::shared_ptr

#include "Timer.h"

/**
 * \brief A class to present the timing results in a table.
 *
 * TimerTree contains one timer object, and a list to child TimerTree objects.
 */
class TimerTree {
    private:
        /**
         * \brief The description of what the timer is timing.
         */
        std::string descr_;
        /**
         * \brief The indentation of the subtimers.
         */
        int indentation_;
        /**
         * \brief The width of the time info box.
         */
        int widthTime_;
        /**
         * \brief The width of the percent info box.
         */
        int widthPercent_;
        /**
         * \brief The timer object of this node.
         */
        std::shared_ptr<Timer> timer_;
        /**
         * \brief The subtree that contain the subtimers.
         */
        std::vector<std::shared_ptr<TimerTree> > children_;
        /**
         * \brief True if the TimerTree has no parent TimerTrees.
         */
        bool root_timer_;
        /**
         * \brief Print the summary of the TimerTree that is child of another
         * TimerTree.
         */
        void summary(int level,
                     size_t total,
                     int nameWidth,
                     int depth) const;
    public:
        /**
         * \brief Construct an empty TimerTree.
         */
        TimerTree();

        TimerTree(const TimerTree & other);

        /**
         * \brief Construct a TimerTree with \a timer Timer.
         */
        TimerTree(const std::string &, std::shared_ptr<Timer> const& timer);
        /**
         *
         */
        TimerTree(const std::string &, bool root_timer = false);
        /**
         * \brief Destructor.
         */
        virtual ~TimerTree();
        /**
         * \brief Set the default values for the printing.
         */
        void setDefaultValues();
        /**
         * \brief Return the name of the timerTree
         */
        std::string descr() const;
        /**
         * \brief Set \a timer to TimerTree.
         */
        void setTimer(std::shared_ptr<Timer> const& timer);
        /**
         * \brief Set the timer description.
         */
        void setDescr(std::string name);
        /**
         * \brief Set \a timer into child TimerTree.
         */
        void addChild(std::shared_ptr<TimerTree> const& timerTree);
        void addChild(const std::string &name, std::shared_ptr<Timer> const& timer);
        std::shared_ptr<TimerTree> addChild(const std::string &name);
        /**
         * \brief Return true, if this TimerTree object has children.
         */
        bool hasChildren() const;
        /**
         * \brief Return the length of the longest \a timingTarget name among
         * this and child objects.
         */
        unsigned int getMaxNameWidth() const;
        /**
         * \brief If this TimerTree has no children, return 0. Otherwise,
         * return 1 + maximum of getMaxDepth() among the children.
         */
        int getMaxDepth() const;
        /**
         * \brief Print the summary.
         */
        void summary() const;
        /**
         * \brief Return the Timer object of this node.
         */
        std::shared_ptr<Timer> getTimer();

        void start();

        void stop();
};


/**
 * \brief RAII timer controller.
 *
 * The TimerController starts the timer when it is created and stop the timer
 * at the time of destruction. The timer can be stopped and started externally
 * if needed.
 */
class TimerController {
    public:
        TimerController() = delete;
        /**
         * \brief Create a controller for the given timer. The timer is
         * started immediately.
         */
        explicit TimerController(std::shared_ptr<TimerTree> const& timerTree);
        /**
         * \brief Destroy the controller. Stop the timer if it is running.
         */
        ~TimerController();
        /**
         * \brief Start the timer. Throw an error if the timer is already
         * running.
         */
        void start();
        /**
         * \brief Stop the timer. Throw an error if the timer is not running.
         */
        void stop();
    private:
        std::shared_ptr<Timer> timer_;
};

#endif /* TIMER_H_ */
