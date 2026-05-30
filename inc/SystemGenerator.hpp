#pragma once

#include <algorithm>

#include "Stellar.hpp"
#include "Planet.hpp"
#include "StellarSystem.hpp"

class SystemGenerator {
public:
    SystemGenerator(const std::string& parameter_file)
    : m_logger(AMB::Logger::instance()), m_parameters_setted(false)
    {
        load_system_parameter(parameter_file);
    }

    StellarSystem generate(uint32_t seed) {
        // WARNING HAS LEHMER ALREADY CHECK STELLAR OBJECT EXISTANCE OR NOT ???

        if (!m_parameters_setted) {
            m_logger.log(AMB::Fatal, "SystemGenerator: Parameters are not set. You must use load_parameter.");
            exit(EXIT_FAILURE);
        }

        // Stellar system object
        StellarSystem system;
        system.seed = seed;
        AMB::Lehmer32 lehmer32(seed);

        // Generate star
        StellarObject stellar = m_stellar_factory.generate_stellar_object(lehmer32);
        system.stellar_object = stellar;

        // Disk
        SystemDisk disk = generate_disk(lehmer32);
        m_slot_weight[2].weight = m_asteroid_belt_weight*disk.instability; // Modify AsteroidBelt weight depending on the instability
        system.disk = disk;

        // Orbital slots count
        std::pair<std::vector<float>, std::vector<SystemSlotContent>> slot_result = generate_slot(lehmer32, disk, stellar);
        system.slot_orbital_distance = slot_result.first;
        system.slot_content = slot_result.second;
        std::vector<float>& slots = system.slot_orbital_distance;
        std::vector<SystemSlotContent>& occupancy = system.slot_content;

        // PLanet generation
        std::vector<PlanetObject>& planets = system.planets;
        for (uint32_t i(0) ; i < slots.size() ; ++i) {
            if (occupancy[i] == SystemSlotContent::Planet) {
                PlanetObject p = m_planet_factory.generate_planet_object(lehmer32);
                p.orbital_radius = slots[i];
                p.seed = seed ^ i;

                // Generate number of moons
                float lambda_0 = 0.5;
                float lambda_inf = 5;
                float lambda_i = 2.5, M_i = 10;

                float b = log(lambda_inf - lambda_0);
                float a = (log(lambda_inf - lambda_i) - b)/(-M_i);

                float lambda = lambda_inf - exp(-a*p.mass + b);
                uint32_t nbr_moons = lehmer32.poisson(lambda);

                float orbit = p.radius*lehmer32.uniform_float(1.5, 2.0);

                // Generate moons
                for (uint32_t j(0) ; j < nbr_moons ; ++j) {
                    MoonObject m = m_moon_factory.generate_moon_object(lehmer32);
                    m.orbital_radius = orbit;
                    m.seed = p.seed ^ j;

                    orbit += p.radius*lehmer32.uniform_float(0.5, 1.5);
                    p.moons.push_back(m);
                }

                planets.push_back(p);

            }else if (occupancy[i] == SystemSlotContent::AsteroidBelt) {

            }else{

            }
        }

        return system;
    }

private:
    StellarObjectFactory m_stellar_factory;
    PlanetObjectFactory m_planet_factory;
    MoonObjectFactory m_moon_factory;
    AMB::Logger& m_logger;

    std::vector<WeightedEntry<SystemSlotContent>> m_slot_weight;
    bool m_parameters_setted;

    uint32_t m_min_slot;
    uint32_t m_max_slot;
    float m_empty_weight;
    float m_planet_weight;
    float m_asteroid_belt_weight;

    float m_asteroid_max_coeff;

    float m_lambda_max;
    float m_lambda_steepness_min;
    float m_lambda_steepness_max;
    float m_lambda_0;

    float m_orbit_init;
    float m_orbital_switch_mode;
    float m_orbital_mult_min;
    float m_orbital_mult_max;

    SystemDisk generate_disk(AMB::Lehmer32& lehmer32) {
        SystemDisk disk;
        disk.richness = lehmer32.next_float32();
        disk.instability = lehmer32.next_float32();

        return disk;
    }

    std::pair<std::vector<float>, std::vector<SystemSlotContent>> generate_slot(AMB::Lehmer32& lehmer32, const SystemDisk& disk, const StellarObject& stellar) {
        float M = m_lambda_max; // Maximum of the lambda
        float a = m_lambda_steepness_max*disk.richness + m_lambda_steepness_min*(1.0f-disk.richness); // Steepness of the curve
        float h = m_lambda_0; // Initial value of lambda at null mass
        float b = 1.0f/a*log(M-h);
        float lambda = M - exp(-a*(stellar.mass-b));
        uint32_t slot_count = lehmer32.poisson(lambda);
        slot_count = std::clamp(slot_count, m_min_slot, m_max_slot);

        std::vector<float> slots; 
        std::vector<SystemSlotContent> occupancy;

        // Orbital slots generation
        float orbit = m_orbit_init; 
        uint32_t max_asteroid_belt = m_asteroid_max_coeff*slot_count;
        uint32_t asteroid_belt_count = 0;
        for (uint32_t i(0) ; i < slot_count ; ++i) {
            if (orbit < m_orbital_switch_mode) {
                orbit *= lehmer32.uniform_float(m_orbital_mult_min, m_orbital_mult_max);
            }else{
                orbit += lehmer32.uniform_float(m_orbital_switch_mode*0.75f, m_orbital_switch_mode*1.25f);
            }
            slots.push_back(orbit);
            SystemSlotContent content = pick_weighted(m_slot_weight, lehmer32);

            // Limit the number of asteroid belt
            if (content == SystemSlotContent::AsteroidBelt) {
                if (asteroid_belt_count < max_asteroid_belt) {
                    asteroid_belt_count += 1;
                }else{
                    content = SystemSlotContent::Empty;
                }
            }
            
            occupancy.push_back(content);
        }

        return {slots, occupancy};
    }

    void set_parameter(const std::string& name, float value) {
        if      (name == "min_slot")                {m_min_slot = value;}
        else if (name == "max_slot")                {m_max_slot = value;}
        else if (name == "empty_weight")            {m_empty_weight = value;}
        else if (name == "planet_weight")           {m_planet_weight = value;}
        else if (name == "asteroid_belt_weight")    {m_asteroid_belt_weight = value;}
        else if (name == "asteroid_max_coeff")      {m_asteroid_max_coeff = value;}
        else if (name == "lambda_max")              {m_lambda_max = value;}
        else if (name == "lambda_steepness_min")    {m_lambda_steepness_min = value;}
        else if (name == "lambda_steepness_max")    {m_lambda_steepness_max = value;}
        else if (name == "lambda_0")                {m_lambda_0 = value;}
        else if (name == "orbit_init")              {m_orbit_init = value;}
        else if (name == "orbital_switch_mode")     {m_orbital_switch_mode = value;}
        else if (name == "orbital_mult_min")        {m_orbital_mult_min = value;}
        else if (name == "orbital_mult_max")        {m_orbital_mult_max = value;}
        else{
            m_logger.log(AMB::Error, "SystemGenerator: Can not set parameter. Name not found. Name: " + name);
        }
    }

    void load_system_parameter(const std::string& file_path) {
        // Open file
        std::fstream file(file_path);
        if (!file.is_open()) {
            m_logger.log(AMB::Fatal, "SystemGenerator: Can not open parameter file: " + file_path);
            exit(EXIT_FAILURE);
        }

        // Read lines
        std::string line1;
        std::string line2;
        std::vector<std::string> header;
        std::vector<float> parameter;

        std::getline(file, line1);
        std::getline(file, line2);

        std::stringstream ss1(line1);
        std::stringstream ss2(line2);

        std::string header_name;
        while(std::getline(ss1, header_name, ',')) {
            header_name.erase(0, header_name.find_first_not_of(" \t"));
            header_name.erase(header_name.find_last_not_of(" \t") + 1);
            header.push_back(header_name);
        }

        std::string param_str;
        while(std::getline(ss2, param_str, ',')) {
            param_str.erase(0, param_str.find_first_not_of(" \t"));
            param_str.erase(param_str.find_last_not_of(" \t") + 1);

            float param;
            try {
                param = std::stof(param_str);
            }catch(...) {
                std::string error_message = "SystemGenerator: Can not cast parameter from str to float: " + param_str;
                m_logger.log(AMB::Fatal, error_message);
                exit(EXIT_FAILURE);
            }

            parameter.push_back(param);
        }

        // Check number of header and parameter
        if (header.size() != parameter.size()) {
            std::string error_message = "SystemGenerator: Not the same number of header and entrier in the parameter file. Number of header: " + 
                std::to_string(header.size()) + "Number of parameter: " + std::to_string(parameter.size());
            m_logger.log(AMB::Fatal, error_message);
            exit(EXIT_FAILURE);
        }

        // Set parameters and check that all parameters are defined
        std::unordered_map<std::string, bool> param_defined = {
            {"min_slot" , false},
            {"max_slot" , false},
            {"empty_weight" , false},
            {"planet_weight" , false},
            {"asteroid_belt_weight" , false},
            {"asteroid_max_coeff" , false},
            {"lambda_max" , false},
            {"lambda_steepness_min" , false},
            {"lambda_steepness_max" , false},
            {"lambda_0" , false},
            {"orbit_init" , false},
            {"orbital_switch_mode" , false},
            {"orbital_mult_min" , false},
            {"orbital_mult_max" , false}
        };
        for (uint32_t i(0) ; i < header.size() ; ++i) {
            std::unordered_map<std::string, bool>::iterator it = param_defined.find(header[i]);

            if (it == param_defined.end()) {
                std::string error_message = "SystemGenerator: Can not find parameter name: " + header[i];
                m_logger.log(AMB::Error, error_message);
            }else if (it->second){
                std::string error_message = "SystemGenerator: Parameter already defined. Name: " + header[i];
                m_logger.log(AMB::Error, error_message);
            }else{
                it->second = true;
                set_parameter(header[i], parameter[i]);
            }
        }

        // Check that all parameters has been defined
        for (auto i : param_defined) {
            if (!i.second) {
                std::string error_message = "SystemGenerator: At least one parameter is undefined. Parameter name: " + i.first;
                m_logger.log(AMB::Error, error_message);
                exit(EXIT_FAILURE);
            }
        }

        // Set final variables
        m_slot_weight.push_back({SystemSlotContent::Empty, m_empty_weight});
        m_slot_weight.push_back({SystemSlotContent::Planet, m_planet_weight});
        m_slot_weight.push_back({SystemSlotContent::AsteroidBelt, m_asteroid_belt_weight});

        m_parameters_setted = true;
        
    }
};