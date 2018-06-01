/**
 * Copyright
 *   2014-2015 Finnish Geodetic Institute
 *   2015-     Finnish Geospatial Research Institute,
 *             National Land Survey of Finland
 *
 * Programmers: David Eränen and Ville Mäkinen
 *
 * This file is released under the GNU Lesser General Public
 * Licence version 2.1.
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

namespace logging {

    /**
     * \brief The indentation of the log lines is increased by a specific
     * amount. The indentation is decreased by the same amount when the object
     * is destroyed.
     */
    class LogIndent {
        public:
            LogIndent();
            ~LogIndent();
            void unIndent();
        private:
            unsigned int indentIncr;
    };

    /**
     * \brief A logging object similar to a streambuffer. The message is
     * printed in a thread-safe fashion when the object is destroyed.
     */
    class LogWriter
    {
        public:
            LogWriter(std::ostream &out);
            LogWriter(LogWriter &&);
            ~LogWriter();
            template<typename T>
            LogWriter & operator<<(const T &t)
            {
                *ss << t;
                return *this;
            }
            void flush();
            std::ostream & stream();
        private:
            std::unique_ptr<std::stringstream> ss;
            std::ostream* stream_;
    };

    void init(const std::string &logfile, bool timed);

    void set_output_to_logfile(const std::string &);
    void set_output_to_stdout();
    void set_output_to_null();

    LogWriter Log();
    LogWriter Err();
    LogWriter pLog();
    LogWriter pLog(int);
    LogWriter pErr();
    LogWriter pLogDB();
    LogWriter plainLog();
}

std::string timestamp();

#endif
