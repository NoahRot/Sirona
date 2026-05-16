#include "Stellar.hpp"

const char* stellar_category_to_string(StellarCategory category) {
    switch (category) {
        case StellarCategory::MainSequence: return "Main Sequence";
        case StellarCategory::Giant:        return "Giant";
        case StellarCategory::WolfRayet:   return "Wolf-Rayet";
        case StellarCategory::WhiteDwarf:  return "White Dwarf";
        case StellarCategory::NeutronStar: return "Neutron Star";
        case StellarCategory::BlackHole:   return "Black Hole";
        case StellarCategory::BrownDwarf:  return "Brown Dwarf";

        default: return "Unknown";
    }
}

StellarCategory string_to_stellar_category(const std::string& str) {
    if (str == "MainSequence") return StellarCategory::MainSequence;
    if (str == "Giant")        return StellarCategory::Giant;
    if (str == "BrownDwarf")   return StellarCategory::BrownDwarf;
    if (str == "WhiteDwarf")   return StellarCategory::WhiteDwarf;
    if (str == "NeutronStar")  return StellarCategory::NeutronStar;
    if (str == "WolfRayet")    return StellarCategory::WolfRayet;
    if (str == "BlackHole")    return StellarCategory::BlackHole;
    
    // Fallback / error case
    throw std::string("WARNING::StellarObjectFactory: Unknown stellar category: " + str);
}

const char* stellar_subtype_to_string(StellarSubtype subtype) {
    switch (subtype)
    {
        // Main sequence
        case StellarSubtype::O: return "O";
        case StellarSubtype::B: return "B";
        case StellarSubtype::A: return "A";
        case StellarSubtype::F: return "F";
        case StellarSubtype::G: return "G";
        case StellarSubtype::K: return "K";
        case StellarSubtype::M: return "M";

        // Giants
        case StellarSubtype::RedGiant:         return "Red Giant";
        case StellarSubtype::BlueGiant:        return "Blue Giant";
        case StellarSubtype::RedSuperGiant:    return "Red Super Giant";
        case StellarSubtype::BlueSuperGiant:   return "Blue Super Giant";
        case StellarSubtype::YellowSuperGiant: return "Yellow Super Giant";
        case StellarSubtype::HyperGiant:       return "Hyper Giant";

        // Brown dwarfs
        case StellarSubtype::L: return "L";
        case StellarSubtype::T: return "T";
        case StellarSubtype::Y: return "Y";

        // White dwarfs
        case StellarSubtype::DA: return "DA";
        case StellarSubtype::DB: return "DB";
        case StellarSubtype::DC: return "DC";
        case StellarSubtype::DO: return "DO";
        case StellarSubtype::DQ: return "DQ";
        case StellarSubtype::DZ: return "DZ";
        case StellarSubtype::DX: return "DX";

        // Wolf-Rayet
        case StellarSubtype::WN: return "WN";
        case StellarSubtype::WC: return "WC";
        case StellarSubtype::WO: return "WO";

        // Neutron stars
        case StellarSubtype::StandardNeutronStar:   return "Standard Neutron Star";
        case StellarSubtype::Pulsar:                return "Pulsar";
        case StellarSubtype::Magnetar:              return "Magnetar";

        // Black holes
        case StellarSubtype::StellarBlackHole:      return "Stellar Black Hole";
        case StellarSubtype::IntermediateBlackHole: return "Intermediate Black Hole";
        case StellarSubtype::SupermassiveBlackHole: return "Supermassive Black Hole";
        case StellarSubtype::PrimordialBlackHole:   return "Primordial Black Hole";

        default: return "Unknown";
    }
}

StellarSubtype string_to_stellar_subtype(const std::string& str) {
    if (str == "O")                    return StellarSubtype::O;
    if (str == "B")                    return StellarSubtype::B;
    if (str == "A")                    return StellarSubtype::A;
    if (str == "F")                    return StellarSubtype::F;
    if (str == "G")                    return StellarSubtype::G;
    if (str == "K")                    return StellarSubtype::K;
    if (str == "M")                    return StellarSubtype::M;

    if (str == "RedGiant")             return StellarSubtype::RedGiant;
    if (str == "BlueGiant")            return StellarSubtype::BlueGiant;
    if (str == "RedSuperGiant")        return StellarSubtype::RedSuperGiant;
    if (str == "BlueSuperGiant")       return StellarSubtype::BlueSuperGiant;
    if (str == "YellowSuperGiant")     return StellarSubtype::YellowSuperGiant;
    if (str == "HyperGiant")           return StellarSubtype::HyperGiant;

    if (str == "L")                    return StellarSubtype::L;
    if (str == "T")                    return StellarSubtype::T;
    if (str == "Y")                    return StellarSubtype::Y;

    if (str == "DA")                   return StellarSubtype::DA;
    if (str == "DB")                   return StellarSubtype::DB;
    if (str == "DC")                   return StellarSubtype::DC;
    if (str == "DO")                   return StellarSubtype::DO;
    if (str == "DQ")                   return StellarSubtype::DQ;
    if (str == "DZ")                   return StellarSubtype::DZ;
    if (str == "DX")                   return StellarSubtype::DX;

    if (str == "WN")                   return StellarSubtype::WN;
    if (str == "WC")                   return StellarSubtype::WC;
    if (str == "WO")                   return StellarSubtype::WO;

    if (str == "StandardNeutronStar")  return StellarSubtype::StandardNeutronStar;
    if (str == "Pulsar")               return StellarSubtype::Pulsar;
    if (str == "Magnetar")             return StellarSubtype::Magnetar;

    if (str == "StellarBlackHole")       return StellarSubtype::StellarBlackHole;
    if (str == "IntermediateBlackHole")  return StellarSubtype::IntermediateBlackHole;
    if (str == "SupermassiveBlackHole")  return StellarSubtype::SupermassiveBlackHole;
    if (str == "PrimordialBlackHole")    return StellarSubtype::PrimordialBlackHole;

    // Fallback / error case (matching your category function style)
    throw std::string("WARNING::StellarObjectFactory: Unknown stellar subtype: " + str);
}

std::string StellarObject::info_str() const {
    std::ostringstream oss;

    oss << "===== Stellar Object =====\n";
    oss << "Name: " << name << '\n';
    oss << "Category: "
        << stellar_category_to_string(category) << '\n';
    oss << "Subtype: "
        << stellar_subtype_to_string(subtype) << '\n';
    oss << "Mass: "
        << mass << " solar masses\n";
    oss << "Radius: "
        << radius << " km\n";
    oss << "Temperature: "
        << temperature << " K\n";

    return oss.str();
}

StellarObjectFactory::StellarObjectFactory() 
: m_logger(AMB::Logger::instance())
{
    // Load categories
    std::ifstream category_file(CATEGORY_PATH);
    if (!category_file.is_open()) {
        std::string error_mess = "StellarObjectFactory: Can not open the stellar category file. File:" + CATEGORY_PATH;
        m_logger.log(AMB::Fatal, error_mess);
        exit(EXIT_FAILURE);
    }

    load_category(category_file);
    category_file.close();

    // Load subtypes
    std::ifstream subtype_file(SUBTYPE_PATH);
    if (!subtype_file.is_open()) {
        std::string error_mess = "StellarObjectFactory: Can not open the stellar subtype file. File:" + SUBTYPE_PATH;
        category_file.close();
        m_logger.log(AMB::Fatal, error_mess);
        exit(EXIT_FAILURE);
    }

    load_subtype(subtype_file);
    subtype_file.close();
}

StellarObject StellarObjectFactory::generate_stellar_object(AMB::Lehmer32& lehmer) {
    StellarObject object;

    // =====================================================
    // CATEGORY
    // =====================================================

    object.category = pick_weighted(m_category_entries, lehmer);

    // =====================================================
    // SUBTYPE
    // =====================================================

    object.subtype = pick_weighted(m_subtype_entries[object.category], lehmer);

    // =====================================================
    // PHYSICAL PROPERTIES
    // =====================================================

    const StellarSubtypeData& data = m_subtype_database[object.subtype];

    object.mass = lehmer.uniform_float(data.min_mass, data.max_mass);
    object.radius = lehmer.uniform_float(data.min_radius, data.max_radius);
    object.temperature = lehmer.uniform_float(data.min_temperature, data.max_temperature);

    return object;
}

std::string StellarObjectFactory::info_category_str() const {
    std::ostringstream oss;
    oss << "===== Stellar Categories =====\n";
    for (const auto& cat : m_category_entries) {
        StellarCategory category = cat.value;
        float probability = cat.weight;

        oss << "Category: " << std::setw(16) << stellar_category_to_string(category) << ", probability: " << std::setw(4) << probability << "\n";
    }

    return oss.str();
}

std::string StellarObjectFactory::info_subtype_str() const {
    std::ostringstream oss;
    oss << "===== Stellar Subtypes =====\n";
    for (auto& type : m_subtype_database) {
        StellarSubtypeData data = type.second;

        oss << "Category: " << std::setw(16) << stellar_category_to_string(data.category) << ", Subtype: " << std::setw(22) << stellar_subtype_to_string(data.subtype)
        << ", Probability: " << std::setw(4) << data.probability << ", Mass [solar mass]: [" << std::setw(4) << data.min_mass << ", " << std::setw(4) << data.max_mass << "]"
        << ", Radius [km]: [" << std::setw(4) << data.min_radius << ", " << std::setw(4) << data.max_radius << "]" 
        << ", Temperature [K]: [" << std::setw(4) << data.min_temperature << ", " << std::setw(4) << data.max_temperature << "]" "\n";
    }

    return oss.str();
}

void StellarObjectFactory::load_category(std::ifstream& category_file) {
    std::string line;
    bool is_first_line = true;

    while (std::getline(category_file, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Skip header
        if (is_first_line) {
            is_first_line = false;
            continue;
        }

        std::stringstream ss(line);
        std::string category_str;
        std::string prob_str;

        // Read category (before comma)
        if (!std::getline(ss, category_str, ',')) continue;
        
        // Read probability (after comma)
        if (!std::getline(ss, prob_str, ',')) continue;

        // Trim whitespace if any
        category_str.erase(0, category_str.find_first_not_of(" \t"));
        category_str.erase(category_str.find_last_not_of(" \t") + 1);
        prob_str.erase(0, prob_str.find_first_not_of(" \t"));
        prob_str.erase(prob_str.find_last_not_of(" \t") + 1);

        StellarCategory cat;
        float probability;
        bool error_category = false;

        try {
            cat = string_to_stellar_category(category_str);
        } catch(std::string error_message) {
            // TODO Log error message
            error_category = true;
        }
        
        try {
            probability = std::stof(prob_str);
        } catch (...) {
            std::string error_message = "StellarObjectFactory: Failed to parse probability for " + category_str + ": " + prob_str;
            m_logger.log(AMB::Warning, error_message);
            error_category = true;
        }

        if (!error_category) {
            m_category_entries.push_back({cat, probability});
        }
    }
}

void StellarObjectFactory::load_subtype(std::ifstream& subtype_file) {
    std::string line;
    bool is_first_line = true;
    std::vector<std::string> header;

    while (std::getline(subtype_file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);

        if (is_first_line) {
            std::string header_name;
            while (std::getline(ss, header_name, ',')) {
                header_name.erase(0, header_name.find_first_not_of(" \t"));
                header_name.erase(header_name.find_last_not_of(" \t") + 1);
                header.push_back(header_name);
            }
            is_first_line = false;
            continue;
        }

        // Reset for this line
        StellarSubtypeData data{};
        StellarSubtype subtype = {};        // Safe default
        bool subtype_found = false;
        uint32_t counter = 0;
        std::string data_str;

        while (std::getline(ss, data_str, ',')) {
            if (counter >= header.size()) break;

            // Trim
            data_str.erase(0, data_str.find_first_not_of(" \t"));
            data_str.erase(data_str.find_last_not_of(" \t") + 1);

            try {
                if (header[counter] == "Category") {
                    data.category = string_to_stellar_category(data_str);
                }
                else if (header[counter] == "Subtype") {
                    subtype = string_to_stellar_subtype(data_str);
                    data.subtype = subtype;
                    subtype_found = true;
                }
                else if (header[counter] == "Probability") {
                    data.probability = std::stof(data_str);
                }
                else if (header[counter] == "MinMassSolar")     data.min_mass = std::stof(data_str);
                else if (header[counter] == "MaxMassSolar")     data.max_mass = std::stof(data_str);
                else if (header[counter] == "MinRadiusKm")      data.min_radius = std::stof(data_str);
                else if (header[counter] == "MaxRadiusKm")      data.max_radius = std::stof(data_str);
                else if (header[counter] == "MinTemperatureK")  data.min_temperature = std::stof(data_str);
                else if (header[counter] == "MaxTemperatureK")  data.max_temperature = std::stof(data_str);
            }
            catch (const std::string& err) {
                std::string error_message("StellarObjectFactory: Subtype loading error (string): " + err);
                m_logger.log(AMB::Error, error_message);
            }
            catch (const std::exception& e) {
                std::string error_message("StellarObjectFactory: Subtype loading parsing error: " + header[counter] + " = '" + data_str + "' : " + e.what());
                m_logger.log(AMB::Error, error_message);
            }
            catch (...) {
                std::string error_message("StellarObjectFactory: UNKNOWN ERROR parsing column");
                m_logger.log(AMB::Error, error_message);
            }

            counter++;
        }

        if (subtype_found) {
            m_subtype_database[subtype] = data;
            m_subtype_entries[data.category].push_back({subtype, data.probability});
        } else {
            std::string error_message("StellarObjectFactory: Subtype not found on this line!");
            m_logger.log(AMB::Warning, error_message);
        }
    }
}