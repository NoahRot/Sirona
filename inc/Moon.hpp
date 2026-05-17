#pragma once

#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <inttypes.h>

#include "WeightedEntry.hpp"

#include "Logger/Logger.hpp"

typedef std::string MoonSubtype;

enum class MoonCategory {
    Rock,
    Ice,
    Volcanic,
    Oceanic,
    Captured,
    Artificial,
    Exotic
};

MoonCategory string_to_moon_category(const std::string& str) {
    if (str == "Telluric")      return MoonCategory::Rock;
    if (str == "Ice")           return MoonCategory::Ice;
    if (str == "Volcanic")      return MoonCategory::Volcanic;
    if (str == "Oceanic")       return MoonCategory::Oceanic;
    if (str == "Captured")      return MoonCategory::Captured;
    if (str == "Exotic")        return MoonCategory::Exotic;
    if (str == "Artificial")    return MoonCategory::Artificial;
    
    // Fallback / error case
    throw std::string("WARNING::StellarObjectFactory: Unknown stellar category: " + str);
}

const char* moon_category_to_string(MoonCategory category) {
    switch (category) {
        case MoonCategory::Rock:        return "Rock";
        case MoonCategory::Ice:         return "Ice";
        case MoonCategory::Volcanic:    return "Volcanic";
        case MoonCategory::Oceanic:     return "Oceanic";
        case MoonCategory::Captured:    return "Captured";
        case MoonCategory::Exotic:      return "Exotic";
        case MoonCategory::Artificial:  return "Artificial";

        default: return "Unknown";
    }
}

struct MoonSubtypeData {
    MoonCategory category;
    MoonSubtype subtype;
    float weight;

    std::unordered_map<std::string, float> data;
};

struct MoonObject {
    std::string name;

    MoonCategory category;
    MoonSubtype subtype;

    float mass;
    float radius;
    float temperature;

    std::vector<std::string> modifiers; // TODO Need to add the modifiers (load csv and generate them)

    std::string info_str() const {
        std::ostringstream oss;

        oss << "===== Moon Object =====\n";
        oss << "Name: " << name << '\n';
        oss << "Category: "
            << moon_category_to_string(category) << '\n';
        oss << "Subtype: "
            << subtype << '\n';
        oss << "Mass: "
            << mass << " earth masses\n";
        oss << "Radius: "
            << radius << " km\n";
        oss << "Temperature: "
            << temperature << " K\n";

        return oss.str();
    }
};

class MoonObjectFactory {
public:
    MoonObjectFactory()
    : m_logger(AMB::Logger::instance())
    {
        load_category();
        load_subtype();
    }

    MoonObject generate_planet_object(AMB::Lehmer32& lehmer) {
        MoonObject object;

        // =====================================================
        // CATEGORY
        // =====================================================

        object.category = pick_weighted(m_category_entries, lehmer);

        // =====================================================
        // SUBTYPE
        // =====================================================

        object.subtype = pick_weighted(m_subtype_entries[object.category], lehmer);

        // =====================================================
        // MODIFIER
        // =====================================================

        // TODO

        // =====================================================
        // PHYSICAL PROPERTIES
        // =====================================================

        MoonSubtypeData data = m_subtype_database[object.subtype];

        object.mass = lehmer.uniform_float(data.data["MinMassEarth"], data.data["MaxMassEarth"]);
        object.radius = lehmer.uniform_float(data.data["MinRadiusKm"], data.data["MaxRadiusKm"]);
        object.temperature = lehmer.uniform_float(data.data["MinTemperatureK"], data.data["MaxTemperatureK"]);

        return object;
    }

    std::string info_category_str() const {
        std::ostringstream oss;
        oss << "===== Planet Categories =====\n";
        for (const auto& cat : m_category_entries) {
            MoonCategory category = cat.value;
            float probability = cat.weight;

            oss << "Category: " << std::setw(16) << moon_category_to_string(category) << ", probability: " << std::setw(4) << probability << "\n";
        }

        return oss.str();
    }

    std::string info_subtype_str() const {
        std::ostringstream oss;
        oss << "===== Planet Subtypes =====\n";
        for (const auto& data : m_subtype_database) {
            MoonCategory category = data.second.category;
            MoonSubtype subtype = data.second.subtype;
            float weight = data.second.weight;

            oss << "Category: " << std::setw(16) << moon_category_to_string(category) << ", subtype: " << std::setw(16) << subtype << ", weight: " << std::setw(4) << weight << "\n    Data (" << data.second.data.size() << "): \n";
            for (auto [element,value] : data.second.data) {
                oss << "     - " << element << " = " << value << "\n";
            }
        }

        return oss.str();
    }

private:
    std::vector<WeightedEntry<MoonCategory>> m_category_entries;
    std::unordered_map<MoonCategory, std::vector<WeightedEntry<MoonSubtype>>> m_subtype_entries;
    std::unordered_map<MoonSubtype, MoonSubtypeData> m_subtype_database;

    const std::string CATEGORY_PATH = "res/data/moon_category.csv";
    const std::string SUBTYPE_PATH = "res/data/moon_subtype.csv";

    AMB::Logger& m_logger;

    void load_category() {
        // Open file
        std::fstream file(CATEGORY_PATH);
        if (!file.is_open()) {
            m_logger.log(AMB::Fatal, "MoonFactory: Can not open category file: " + CATEGORY_PATH);
            exit(EXIT_FAILURE);
        }

        // Read lines
        std::string line;
        bool first_line = true;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            // Skip header
            if (first_line) {
                first_line = false;
                continue;
            }

            std::stringstream ss(line);
            std::string category_str, weight_str;
            std::getline(ss, category_str, ',');
            std::getline(ss, weight_str, ',');
            bool error_category(false);
            float weight;
            MoonCategory category;

            // Extract category
            try {
                category = string_to_moon_category(category_str);
            }
            catch(std::string e) {
                error_category = true;
                m_logger.log(AMB::Warning, e);
            }
            
            // Extract weight
            try {
                weight = std::stof(weight_str);
            } catch (...) {
                std::string error_message = "MoonFactory: Failed to parse probability for " + category_str + ": " + weight_str;
                m_logger.log(AMB::Warning, error_message);
                error_category = true;
            }

            // Store category 
            if (!error_category) {
                m_category_entries.push_back({category, weight});
            }
        }
    }

    void load_subtype() {
        // Open file
        std::fstream file(SUBTYPE_PATH);
        if (!file.is_open()) {
            m_logger.log(AMB::Fatal, "MoonFactory: Can not open subtype file: " + SUBTYPE_PATH);
            exit(EXIT_FAILURE);
        }

        // Read lines
        std::string line;
        bool first_line = true;
        std::vector<std::string> header;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);

            // read header
            if (first_line) {
                first_line = false;
                std::string header_name;
                while(std::getline(ss, header_name, ',')) {
                    header_name.erase(0, header_name.find_first_not_of(" \t"));
                    header_name.erase(header_name.find_last_not_of(" \t") + 1);
                    header.push_back(header_name);
                }

                // Sanity check
                if (header[0] != "Subtype" or header[1] != "Category" or header[2] != "Weight") {
                    std::string error_message = "MoonFactory: Subtype file do not have a correct layout. Column one must be 'Subtype', column two must be 'Category', column two must be 'Weight'";
                    m_logger.log(AMB::Fatal, error_message);
                    exit(EXIT_FAILURE);
                }
                continue;
            }

            std::string subtype_str, category_str, weight_str, element_str;
            float weight;
            bool error_subtype(false);
            MoonSubtypeData data;

            // Get subtype
            std::getline(ss, subtype_str, ','); 
            subtype_str.erase(0, subtype_str.find_first_not_of(" \t"));
            subtype_str.erase(subtype_str.find_last_not_of(" \t") + 1);
            MoonSubtype subtype = subtype_str;
            data.subtype = subtype;

            // Get category
            std::getline(ss, category_str, ',');
            category_str.erase(0, category_str.find_first_not_of(" \t"));
            category_str.erase(category_str.find_last_not_of(" \t") + 1);
            MoonCategory category = string_to_moon_category(category_str);
            data.category = category;

            // Get weight
            std::getline(ss, weight_str, ',');
            weight_str.erase(0, weight_str.find_first_not_of(" \t"));
            weight_str.erase(weight_str.find_last_not_of(" \t") + 1);
            try {
                weight = std::stof(weight_str);
                data.weight = weight;
            }catch(...) {
                std::string error_message = "MoonFactory: Can not read weight of subtype: " + subtype_str;
                m_logger.log(AMB::Error, error_message);
                error_subtype = true;
            }

            // Get other entries
            uint32_t counter = 3;
            while(std::getline(ss, element_str, ',')) {
                element_str.erase(0, element_str.find_first_not_of(" \t"));
                element_str.erase(element_str.find_last_not_of(" \t") + 1);
                float element;

                try {
                    element = std::stof(element_str);
                    data.data[header[counter]] = element;
                }catch(...) {
                    std::string error_message = "MoonFactory: Can not read subtype parameter: " + header[counter] + " = " + element_str;
                    m_logger.log(AMB::Error, error_message);
                    error_subtype = true;
                }

                counter++;
            }

            if (!error_subtype) {
                m_subtype_entries[category].push_back({subtype, weight});
                m_subtype_database[subtype] = data;
            }
        }

        // Check each subtype database contains all header
        for (auto [subtype, data] : m_subtype_database) {
            for (uint32_t i(3) ; i < header.size() ; ++i) {
                std::string h = header[i];
                if (data.data.find(h) == data.data.end()) {
                    std::string error_message = "MoonFactory: Element is missing in at least on subtype. Subtype=" + subtype + ", element=" + h;
                    m_logger.log(AMB::Fatal, error_message);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
};