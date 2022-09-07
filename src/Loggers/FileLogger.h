#ifndef PHYSICSSIMULATION_FILELOGGER_H
#define PHYSICSSIMULATION_FILELOGGER_H

#include <filesystem>
#include <fstream>
#include <utility>

#include "Logger.h"

class FileLogger : public Logger {
private:
    std::filesystem::path m_path;
    std::ofstream m_ofstream;

public:
    explicit FileLogger(std::filesystem::path path) : m_path(std::move(path)) {
        m_ofstream.open(m_path, std::ios_base::app);

        m_ofstream << "Total energy:\n";

        m_ofstream.close();
    }

    void log(const World &world, int iteration) override {
        m_ofstream.open(m_path, std::ios_base::app);

        m_ofstream << std::fixed << std::setprecision(9) << world.getTotalEnergy() << "\n";

        m_ofstream.close();
    }
};


#endif //PHYSICSSIMULATION_FILELOGGER_H
