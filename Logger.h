#ifndef PHYSICSSIMULATION_LOGGER_H
#define PHYSICSSIMULATION_LOGGER_H

#include <filesystem>
#include <fstream>
#include <utility>

class Logger {
private:
    std::filesystem::path m_path;
    std::ofstream m_ofstream;

public:
    explicit Logger(std::filesystem::path path) : m_path(std::move(path)) {
        m_ofstream.open(m_path, std::ios_base::app);

        m_ofstream << "Temperature, Pressure, Area, Atoms count\n";

        m_ofstream.close();
    }

    void log(double temperature, double pressure, double area, std::size_t atoms_count) {
        m_ofstream.open(m_path, std::ios_base::app);

        m_ofstream << temperature << ", " << pressure << ", " << area << ", " << atoms_count << "\n";

        m_ofstream.close();
    }
};


#endif //PHYSICSSIMULATION_LOGGER_H
