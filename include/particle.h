#pragma once
#include <vector>

enum Role { ER, EI }; // Exploration or Exploitation

struct Particle {
    std::vector<double> x;      // Position
    std::vector<double> v;      // Velocity
    std::vector<double> pbest;  // Personal best (not used in CHxPSO, kept for compatibility)
    double pbest_fit;
    Role role;
    
    // Employment counter (α_Er or α_Ei)
    int alpha;
};

struct Layer {
    Particle Er;                    // Exploration particle
    Particle Ei;                    // Exploitation particle
    std::vector<double> L;          // Single-layer best vector
    double L_fit;                   // Fitness of L
    std::vector<double> Q;          // Constructed exemplar vector
    int beta;                       // Counter for L updates by Er particle (at layer level)
};