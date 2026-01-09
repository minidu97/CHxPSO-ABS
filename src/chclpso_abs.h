#pragma once
#include "chxpso_abs.h"
#include <vector>

// CHCLPSO-ABS: CLPSO with Comprehensive Learning and ABS
class CHCLPSO_ABS : public CHxPSO_ABS {
private:
    double Pc_init = 0.05;
    double Pc_final = 0.5;
    std::vector<std::vector<int>> learning_source;  // Which layer each dimension learns from
    
    double getPc() {
        // Pc increases linearly: 0.05 -> 0.5
        return Pc_init + (Pc_final - Pc_init) * static_cast<double>(FEs) / max_FEs;
    }
    
public:
    CHCLPSO_ABS(int N_, int dim_, int max_FEs_, int M_)
        : CHxPSO_ABS(N_, dim_, max_FEs_, M_) {
        learning_source.resize(N, std::vector<int>(dim, 0));
    }
    
    // Override constructQ: use CL strategy
    void constructQ(int layer_idx) override {
        Layer& layer = layers[layer_idx];
        double Pc = getPc();
        
        for(int d = 0; d < dim; ++d) {
            if(dis(gen) < Pc) {
                // Learn from a random layer's L vector
                int random_layer = static_cast<int>(dis(gen) * N);
                if(random_layer >= N) random_layer = N - 1;
                
                layer.Q[d] = layers[random_layer].L[d];
                learning_source[layer_idx][d] = random_layer;
            } else {
                // Learn from own L vector
                layer.Q[d] = layer.L[d];
                learning_source[layer_idx][d] = layer_idx;
            }
        }
    }
};