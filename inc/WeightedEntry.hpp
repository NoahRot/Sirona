#pragma once

#include <vector>

#include "Random/Lehmer.hpp"

/// @brief Entry with a weight associated. Allow to easily pick a random entry according to their weigth
/// @tparam T Type of the entry
template<typename T>
struct WeightedEntry {
    T value;
    float weight;
};

/// @brief Randomly pick up weighted entry 
/// @tparam T Type of teh entry
/// @param entries Vector containing the entries
/// @param lehmer Random generator
/// @return The entry randomly selected
template<typename T>
T pick_weighted(const std::vector<WeightedEntry<T>>& entries, AMB::Lehmer32& lehmer) {
    float total_weight = 0.0f;

    for (const auto& e : entries) {
        total_weight += e.weight;
    }

    const float roll = lehmer.next_float32() * total_weight;

    float accumulator = 0.0f;

    for (const auto& e : entries) {
        accumulator += e.weight;

        if (roll <= accumulator) {
            return e.value;
        }
    }

    // Safety fallback
    return entries.back().value;
}
