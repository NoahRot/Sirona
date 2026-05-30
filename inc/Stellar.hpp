#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <inttypes.h>
#include <vector>

#include "Logger/Logger.hpp"
#include "Random/Lehmer.hpp"

#include "WeightedEntry.hpp"
#include "Global.hpp"

/// @brief Stellar category of objects
enum class StellarCategory {
    MainSequence,
    Giant,
    WolfRayet,
    WhiteDwarf,
    NeutronStar,
    BlackHole,
    BrownDwarf
};

/// @brief Return a string containing the name of the stellar category
/// @param category The stellar category
/// @return A string containing the name
const char* stellar_category_to_string(StellarCategory category);

/// @brief Return the category corresponding to the string 
/// @param str The string
/// @return The category
/// @throw A string with the error message
StellarCategory string_to_stellar_category(const std::string& str);

/// @brief Subtype
enum class StellarSubtype
{
    // =========================
    // Main sequence
    // =========================
    O,
    B,
    A,
    F,
    G,
    K,
    M,

    // =========================
    // Giants
    // =========================
    RedGiant,
    BlueGiant,
    RedSuperGiant,
    BlueSuperGiant,
    YellowSuperGiant,
    HyperGiant,

    // =========================
    // Brown dwarfs
    // =========================
    L,
    T,
    Y,

    // =========================
    // White dwarfs
    // =========================
    DA,
    DB,
    DC,
    DO,
    DQ,
    DZ,
    DX,

    // =========================
    // Wolf-Rayet
    // =========================
    WN,
    WC,
    WO,

    // =========================
    // Neutron stars
    // =========================
    StandardNeutronStar,
    Pulsar,
    Magnetar,

    // =========================
    // Black holes
    // =========================
    StellarBlackHole,
    IntermediateBlackHole,
    SupermassiveBlackHole,
    PrimordialBlackHole
};

/// @brief Return a string containing the name of the stellar subtype
/// @param subtype The stellar subtype
/// @return A string containing the name
const char* stellar_subtype_to_string(StellarSubtype subtype);

/// @brief Return the subtype corresponding to the string
/// @param str The string
/// @return The subtype
/// @throw A string with the error message
StellarSubtype string_to_stellar_subtype(const std::string& str);


/// @brief Stellar object class
struct StellarObject
{
    StellarCategory category;
    StellarSubtype subtype;

    std::string name;

    float mass;
    float radius;
    float temperature;
    float luminosity;

    /// @brief Return the informations of the stellar object in a string
    /// @return The string containing the informations of the stellar object
    std::string info_str() const;
};

/// @brief Data linked with a subtype. Give probability and physical parameters range.
struct StellarSubtypeData
{
    StellarCategory category;
    StellarSubtype subtype;

    float probability;

    float min_mass;
    float max_mass;

    float min_radius;
    float max_radius;

    float min_temperature;
    float max_temperature;

    float min_luminosity;
    float max_luminosity;
};

/// @brief Factory to create stellar objects
class StellarObjectFactory {
public:

    /// @brief Constructor
    StellarObjectFactory();

    /// @brief Generate a random stellar object
    /// @param lehmer Random number generator
    /// @return A randomly created stellar object
    StellarObject generate_stellar_object(AMB::Lehmer32& lehmer);

    /// @brief Return the informations of the categories in a string
    /// @return String with the informations
    std::string info_category_str() const;

    /// @brief Return the informations of the subtype in a string
    /// @return String with the informations
    std::string info_subtype_str() const;

private:
    const std::string CATEGORY_PATH = "res/data/stellar_category_probabilities.csv";
    const std::string SUBTYPE_PATH = "res/data/stellar_subtype_probabilities.csv";

    std::vector<WeightedEntry<StellarCategory>> m_category_entries;
    std::unordered_map<StellarCategory, std::vector<WeightedEntry<StellarSubtype>>> m_subtype_entries;
    std::unordered_map<StellarSubtype, StellarSubtypeData> m_subtype_database;

    AMB::Logger& m_logger;

    /// @brief Load categories from the CSV file
    /// @param category_file CSV file containing the categories
    void load_category(std::ifstream& category_file);

    /// @brief Load subtypes from the CSV file
    /// @param subtype_file CSV file containing the subtypes
    void load_subtype(std::ifstream& subtype_file);
};