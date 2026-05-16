#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <array>
#include <vector>
#include <inttypes.h>

#include "Logger/Logger.hpp"

enum class TagCategory {
    Environmental,
    Biological,
    Geological,
    Hazard,
    Civilization,
    Generation,
    Gameplay
};

TagCategory string_to_tag_category(const std::string& str) {
    if (str == "Environmental") return TagCategory::Environmental;
    if (str == "Biological")    return TagCategory::Biological;
    if (str == "Geological")    return TagCategory::Geological;
    if (str == "Hazard")        return TagCategory::Hazard;
    if (str == "Civilization")  return TagCategory::Civilization;
    if (str == "Generation")    return TagCategory::Generation;
    if (str == "Gameplay")      return TagCategory::Gameplay;
    
    // Fallback / error case
    throw std::string("TagCategory: Unknown tag category: " + str);
}

const char* tag_category_to_string(TagCategory category) {
    switch (category) {
        case TagCategory::Environmental: return "Environmental";
        case TagCategory::Biological   : return "Biological";
        case TagCategory::Geological   : return "Geological";
        case TagCategory::Hazard       : return "Hazard";
        case TagCategory::Civilization : return "Civilization";
        case TagCategory::Generation   : return "Generation";
        case TagCategory::Gameplay     : return "Gameplay";

        default: return "Unknown";
    }
}

struct Tag {
    uint32_t id;
    TagCategory category;

    bool operator==(const Tag& t) {
        return id == t.id;
    }
};

typedef std::vector<Tag> TagList;

bool has_tag(const TagList& list, uint32_t id) {
    for (const auto& t : list) {
        if (t.id == id) {
            return true;
        }
    }

    return false;
}

bool has_tag(const TagList& list, Tag tag) {
    return has_tag(list, tag.id);
}

class TagManager {
public:
    TagManager()
    : m_next_id(0), m_logger(AMB::Logger::instance())
    {}

    bool exist(const std::string& name) {
        return m_tag_table.find(name) != m_tag_table.end();
    }

    bool create(const std::string& name, TagCategory category) {
        if (exist(name)) {
            m_logger.log(AMB::Warning, "TagManager: Try to create an already existing tag. Tag: " + name);
            return false;
        }

        m_tag_table[name] = Tag{m_next_id, category};
        m_next_id++;
        return true;
    }

    Tag get(const std::string& name) {
        return m_tag_table[name];
    }

    std::string info_tag_str() {
        std::ostringstream oss;

        oss << "===== Tags =====\n";
        for (auto& t : m_tag_table) {
            oss << "    Tag: " << std::setw(16) << t.first << ", Category:" << std::setw(16) << tag_category_to_string(t.second.category) << ", ID: " << std::setw(4) << t.second.id << "\n";
        }

        return oss.str();
    }

    void load_tag_csv(const std::string& file_path) {
        // Open file
        std::fstream file(file_path);
        if (!file.is_open()) {
            m_logger.log(AMB::Error, "TagManager: Can not open tag file: " + file_path);
            return;
        }

        // Read lines
        std::string line;
        bool first_line = true;

        while (std::getline(file, line)) {

            std::stringstream ss(line);
            std::string tag_str;
            std::string category_str;

            // Skip header
            if (first_line) {
                first_line = false;
                continue;
            }

            // Extract data
            std::getline(ss, tag_str, ',');
            tag_str.erase(0, tag_str.find_first_not_of(" \t"));
            tag_str.erase(tag_str.find_last_not_of(" \t") + 1);
            std::getline(ss, category_str, ',');
            category_str.erase(0, category_str.find_first_not_of(" \t"));
            category_str.erase(category_str.find_last_not_of(" \t") + 1);

            TagCategory category;

            // Retrieve tag category
            try{
                category = string_to_tag_category(category_str);
                create(tag_str, category);
            }catch(std::string) {
                m_logger.log(AMB::Error, "TagManager: Unvalid tag category: " + category_str);
            }
        }
    }

private:
    uint32_t m_next_id;
    std::unordered_map<std::string, Tag> m_tag_table;
    AMB::Logger& m_logger;
};