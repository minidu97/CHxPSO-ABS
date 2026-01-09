#pragma once
#include <vector>
#include <cmath>
#include "particle.h"

class ABSStrategy {
    int M;  // Total upper threshold
public:
    ABSStrategy(int M_) : M(M_) {}
    
    // Calculate M_Er and M_Ei according to equation (7)
    void calculateThresholds(int FEs, int FEs_max, int& M_Er, int& M_Ei) {
        double ratio = static_cast<double>(FEs) / FEs_max;
        
        // M_Er = ceil(M * (1 - FEs/FEs_max))
        M_Er = static_cast<int>(std::ceil(M * (1.0 - ratio)));
        
        // M_Ei = floor(M * (FEs/FEs_max))
        M_Ei = static_cast<int>(std::floor(M * ratio));
    }
    
    // Determine which particle (Er or Ei) should be active for this layer
    // Returns: 0 = reconstruct Q, 1 = use Er, 2 = use Ei
    int selectParticle(Layer& layer, int M_Er, int M_Ei) {
        // Condition 3 (reconstruction): (β ≠ 0 && α_Er > M_Er) || α_Ei > M_Ei
        if ((layer.beta != 0 && layer.Er.alpha > M_Er) || layer.Ei.alpha > M_Ei) {
            return 0; // Reconstruct
        }
        
        // Condition 1 (exploration): α_Er <= M_Er
        if (layer.Er.alpha <= M_Er) {
            return 1; // Use Er particle
        }
        
        // Condition 2 (exploitation): (α_Er > M_Er && β == 0) && α_Ei <= M_Ei
        if ((layer.Er.alpha > M_Er && layer.beta == 0) && layer.Ei.alpha <= M_Ei) {
            return 2; // Use Ei particle
        }
        
        return 0; // Default to reconstruct
    }
};