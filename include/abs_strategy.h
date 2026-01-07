#pragma once
#include "particle.h"
#include <vector>

class ABSStrategy {
private:
    int M; // parameter controlling particle ratio
public:
    ABSStrategy(int M_) : M(M_) {}
    
    void updateActiveSet(int iter, int maxIter, const std::vector<Particle>& swarm,
                         std::vector<int>& active_indices) {
        active_indices.clear();
        int swarm_size = swarm.size();
        double ratio = 0.5 + 0.5 * iter / maxIter; // simple adaptive ratio
        int active_count = (int)(swarm_size * ratio);
        for(int i=0; i<active_count; ++i)
            active_indices.push_back(i);
    }
};
