#pragma once

#include "Planet.hpp"
#include "Moon.hpp"
#include "Stellar.hpp"

#include "Graphic/VertexArray.hpp"
#include "Graphic/IndexBuffer.hpp"
#include "Graphic/VertexBuffer.hpp"
#include "Graphic/Layout.hpp"

struct SystemDisk {
    float richness;
    float instability;
};

enum class SystemOrbitalZone {
    Inner,
    Habitable,
    Outer,
    Frozen
};

enum class SystemSlotContent {
    Empty,
    Planet,
    AsteroidBelt
};

struct StellarSystem {
    uint32_t seed;

    StellarObject stellar_object;
    std::vector<PlanetObject> planets;
    SystemDisk disk;
    std::vector<float> slot_orbital_distance;
    std::vector<SystemSlotContent> slot_content;

    std::string info_str() {
        std::stringstream oss;
        oss << "===== Stellar System =====\n";
        oss << stellar_object.info_str();
        oss << "Disk: \n > Richness: " << disk.richness << "\n > Instability: "  << disk.instability << "\n";
        oss << "Slots: \n > slot count: " << slot_orbital_distance.size() << "\n";

        std::string debug = "Stellar system layout:\n";
        uint32_t planet_count(0); 
        for (uint32_t i(0) ; i < slot_orbital_distance.size() ; ++i) {
            std::string occ_debug;
            uint32_t nbr_moon(0);
            std::string data_str = "";
            switch(slot_content[i]) {
                case SystemSlotContent::Empty:
                    occ_debug = "Empty";
                    break;
                case SystemSlotContent::Planet:
                    occ_debug = "Planet";
                    nbr_moon = planets[planet_count].moons.size();
                    data_str += planets[planet_count].info_str(3);
                    for (uint32_t j(0) ; j < planets[planet_count].moons.size() ; ++j) {
                        data_str += planets[planet_count].moons[j].info_str(6);
                    }
                    planet_count++;
                    break;
                case SystemSlotContent::AsteroidBelt:
                    occ_debug = "Asteroid Belt";
                    break;
            }

            debug += " > Object " + std::to_string(i+1) + "/" + std::to_string(slot_orbital_distance.size()) + 
            ", Orbital: " + std::to_string(slot_orbital_distance[i]) + 
            ", Content: " + occ_debug + 
            ", Nbr moons: " + std::to_string(nbr_moon) + 
            "\n" + data_str + " \n";

            oss << debug;
        }
        
        return oss.str();
    }
};

struct StellarVisual {

};

struct PlanetVisual {

};

struct MoonVisual {

};

class StellarSystemSceneBuilder {
public:

private:

};