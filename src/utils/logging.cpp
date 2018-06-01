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

#include "logging.h"
#include <iomanip>
#include <sstream>

#include "global_parameters.h"


namespace {

    std::string output_identifier;
    std::ostream *o;
    std::ofstream outFileStream;
    std::ofstream nullOutStream;
    std::string prefix;
    int indent;
    bool timed;

}

namespace logging {

    std::string current_time();

    void close_old_output();

    LogWriter::LogWriter(std::ostream &out):
        stream_ {&out}
    {
        ss.reset(new std::stringstream);
    }

    LogWriter::LogWriter(LogWriter && old)
    {
        std::swap(ss, old.ss);
        stream_ = old.stream_;
    }

    LogWriter::~LogWriter()
    {
        #ifdef USE_OMP
        #pragma omp critical(print)
        #endif
        if (ss) {
            *stream_ << ss->str() << std::endl;
        }
    }

    void LogWriter::flush()
    {
        #ifdef USE_OMP
        #pragma omp critical(print)
        #endif
        if (ss) {
            *stream_ << ss->str() << std::flush;
            ss.reset(new std::stringstream());
        }
    }

    std::ostream & LogWriter::stream()
    {
        if (!ss) {
            throw std::runtime_error("The LogWriter object has no stream buffer.");
        }
        return *ss;
    }

    LogIndent::LogIndent(): indentIncr(2)
    {
        indent += static_cast<int>(indentIncr);
    }
    LogIndent::~LogIndent()
    {
        unIndent();
    }
    void LogIndent::unIndent()
    {
        indent -= static_cast<int>(indentIncr);
        indentIncr = 0;
    }

    void close_old_output()
    {
        if (output_identifier == std::string("")) return;

        if (output_identifier == std::string("-")) return;

        outFileStream.close();
        o = nullptr;
    }

    void set_output_to_logfile(const std::string &out)
    {
        close_old_output();
        output_identifier = std::string(out);
        std::stringstream filename;
        filename << out;
        outFileStream.open(filename.str().c_str(),
            std::ofstream::binary | std::ofstream::out | std::ofstream::app);
        if (outFileStream.is_open()) {
            o = &outFileStream;
        } else {
            o = &std::cerr;
            *o << "could not open logfile '" << out << "'" << std::endl;
        }
    }

    void set_output_to_stdout()
    {
        close_old_output();
        output_identifier = std::string("-");
        o = &std::cout;
    }

    void set_output_to_null()
    {
        close_old_output();
        output_identifier = std::string("null");
        o = &nullOutStream;
    }

    void init(const std::string &out, bool timed_)
    {
        indent = 0;
        timed = timed_;
        prefix = std::string("");
        if (out == "-") {
            set_output_to_stdout();
        } else if (out == "null") {
            set_output_to_null();
        } else {
            set_output_to_logfile(out);
        }
    }

    LogWriter Log()
    {
        LogWriter logger(*o);
        if (timed)
            logger << current_time() << " ";
        return logger;
    }

    LogWriter pLog()
    {
        LogWriter logger(*o);
        logger << prefix;
        if (timed) logger << current_time() << " ";
        logger << std::setw(indent) << "";
        return logger;
    }

    LogWriter pLog(int verb_level)
    {
        if (global_parameters::verbosity >= verb_level) {
            return pLog();
        } else {
            return LogWriter(nullOutStream);
        }
    }

    LogWriter pLogDB()
    {
        return std::move(pLog() << "DEBUG: ");
    }

    LogWriter Err()
    {
        return std::move(Log() << "ERROR: ");
    }

    LogWriter pErr()
    {
        return std::move(pLog() << "ERROR: ");
    }

    LogWriter plainLog()
    {
        return LogWriter(*o);
    }

    std::string current_time() {
        time_t t = std::time(0);
        struct tm now;
        now = *std::localtime(&t);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << now.tm_hour << ":"
            << std::setw(2) << now.tm_min << ":"
            << std::setw(2) << now.tm_sec;
        return ss.str();
    }

}

std::string timestamp() {
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char timechar[32];
    strftime(timechar, 32, "[%T]", timeinfo);

    return std::string(timechar);
}
