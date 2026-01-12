#pragma once
#include "chxpso_abs.h"

//CHpPSO-ABS-Cognitive-only PSO with ABS
//Q is simply the single-layer best vector L
class CHpPSO_ABS : public CHxPSO_ABS {
public:
    CHpPSO_ABS(int N_, int dim_, int max_FEs_, int M_)
        : CHxPSO_ABS(N_, dim_, max_FEs_, M_) {}
    
    //Override constructQ: use own L vector
    void constructQ(int layer_idx) override {
        layers[layer_idx].Q = layers[layer_idx].L;
    }
};