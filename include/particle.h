#pragma once
#include <vector>

enum ParticleRole { ER, EI }; // Exploration or Exploitation

struct Particle {
    std::vector<double> x;      // Position
    std::vector<double> v;      // Velocity
    std::vector<double> pbest;  // Personal best position
    double pbest_fit;           // Personal best fitness
    ParticleRole role;          // Single-role specialization
};
