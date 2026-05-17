#pragma once

#include <algorithm>

#include "Stellar.hpp"
#include "Planet.hpp"
#include "Tag.hpp"

float calculate_expected_planets(StellarCategory category, float star_mass_solar) {
    float lambda;
    switch (category)
    {
        case StellarCategory::MainSequence:
            lambda = 6.0f;
            break;

        case StellarCategory::Giant:
            lambda = 8.0f;
            break;

        case StellarCategory::BrownDwarf:
            lambda = 3.0f;
            break;

        case StellarCategory::NeutronStar:
            lambda = 4.0f;
            break;

        case StellarCategory::BlackHole:
            lambda = 5.0f;
            break;

        default:
            lambda = 4.0f;
            break;
    }

    lambda *= std::sqrt(star_mass_solar);
    lambda = std::clamp(lambda, 0.5f, 15.0f);

    return lambda;
}

void generate_system(uint32_t seed, const TagList& galaxy_tag) {
    // Prepare generator
    AMB::Lehmer32 lehmer32(seed);

    // Generate stellar object
    StellarObjectFactory stellar_factory;
    StellarObject stellar = stellar_factory.generate_stellar_object(lehmer32);

    // Generate planets
    float expected_planets = calculate_expected_planets(stellar.category, stellar.mass);
    int nbr_planets = lehmer32.poisson(expected_planets);

    std::cout << stellar.info_str() << "\n" << "number of planets: " << nbr_planets << std::endl;
}

struct StellarSystem {
    uint32_t seed;

    StellarObject stellar_object;
};

class SystemGenerator {
public:

private:

};