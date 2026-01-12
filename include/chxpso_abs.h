#pragma once
#include "particle.h"
#include "abs_strategy.h"
#include <vector>
#include <functional>
#include <random>
#include <iostream>
#include <algorithm>
#include <cmath>

class CHxPSO_ABS {
protected:
    int N;                      //Number of layers (population size)
    int dim;
    int max_FEs;                //Maximum function evaluations
    int M;                      //Total upper threshold
    int FEs;                    //Current function evaluations
    
    std::vector<Layer> layers;
    std::vector<double> gbest;
    double gbest_fit;
    ABSStrategy abs;
    
    std::function<double(const std::vector<double>&)> objective;
    
    std::mt19937 gen;
    std::uniform_real_distribution<double> dis;
    
    double x_min = -100.0;
    double x_max = 100.0;
    double v_max = 0.2 * (x_max - x_min);  //Typical PSO velocity limit
    
    //Time-varying parameters (initial and final values)
    double w_init = 0.99, w_final = 0.2;
    double c_init = 3.0, c_final = 1.5;
    double c1_init = 2.5, c1_final = 0.5;
    
    //Calculate linearly decreasing parameter
    double getLinearParam(double init, double final, int current_FEs, int max_FEs) {
        return init - (init - final) * static_cast<double>(current_FEs) / max_FEs;
    }
    
public:
    CHxPSO_ABS(int N_, int dim_, int max_FEs_, int M_)
        : N(N_), dim(dim_), max_FEs(max_FEs_), M(M_), FEs(0),
          gbest_fit(1e9), abs(M_), dis(0.0, 1.0) {
        gen.seed(std::random_device{}());
        gbest.resize(dim);
        layers.resize(N);
    }
    
    virtual ~CHxPSO_ABS() {}
    
    void setObjective(std::function<double(const std::vector<double>&)> obj) {
        objective = obj;
    }
    
    void setBounds(double min_val, double max_val) {
        x_min = min_val;
        x_max = max_val;
        v_max = 0.2 * (x_max - x_min);
    }
    
    void initialize() {
        //Step 1: Generate initial swarm randomly
        for(int n = 0; n < N; ++n) {
            Layer& layer = layers[n];
            
            //Initialize positions and velocities
            layer.Er.x.resize(dim);
            layer.Er.v.resize(dim);
            layer.Ei.x.resize(dim);
            layer.Ei.v.resize(dim);
            layer.L.resize(dim);
            layer.Q.resize(dim);
            
            for(int d = 0; d < dim; ++d) {
                double x_init = x_min + dis(gen) * (x_max - x_min);
                double v_init = -v_max + dis(gen) * 2 * v_max;
                
                //Duplication: both Er and Ei start with same position/velocity
                layer.Er.x[d] = x_init;
                layer.Er.v[d] = v_init;
                layer.Ei.x[d] = x_init;
                layer.Ei.v[d] = v_init;
                
                layer.L[d] = x_init;
            }
            
            //Evaluate initial fitness
            layer.L_fit = objective(layer.L);
            FEs++;
            
            //Initialize counters
            layer.Er.alpha = 0;
            layer.Ei.alpha = 0;
            layer.beta = 0;
            
            //Set roles
            layer.Er.role = ER;
            layer.Ei.role = EI;
            
            //Update global best
            if(layer.L_fit < gbest_fit) {
                gbest_fit = layer.L_fit;
                gbest = layer.L;
            }
        }
        
        //Construct initial Q vectors
        for(int n = 0; n < N; ++n) {
            constructQ(n);
        }
    }
    
    //Virtual function to construct Q - to be overridden by derived classes
    virtual void constructQ(int layer_idx) {
        // Default: use own L vector (basic cognitive-only PSO)
        layers[layer_idx].Q = layers[layer_idx].L;
    }
    
    //Er-channel update: equation (5)
    void updateErChannel(int layer_idx) {
        Layer& layer = layers[layer_idx];
        double w = getLinearParam(w_init, w_final, FEs, max_FEs);
        double c = getLinearParam(c_init, c_final, FEs, max_FEs);
        
        for(int d = 0; d < dim; ++d) {
            double r = dis(gen);
            layer.Er.v[d] = w * layer.Er.v[d] + c * r * (layer.Q[d] - layer.Er.x[d]);
            
            //Velocity clamping
            layer.Er.v[d] = std::max(-v_max, std::min(v_max, layer.Er.v[d]));
            
            //Position update
            layer.Er.x[d] += layer.Er.v[d];
            
            //Boundary handling
            if(layer.Er.x[d] < x_min) {
                layer.Er.x[d] = x_min;
                layer.Er.v[d] = 0;
            }
            if(layer.Er.x[d] > x_max) {
                layer.Er.x[d] = x_max;
                layer.Er.v[d] = 0;
            }
        }
    }
    
    //Ei-channel update: equation (6)
    void updateEiChannel(int layer_idx) {
        Layer& layer = layers[layer_idx];
        double w = getLinearParam(w_init, w_final, FEs, max_FEs);
        double c1 = getLinearParam(c1_init, c1_final, FEs, max_FEs);
        
        for(int d = 0; d < dim; ++d) {
            double r1 = dis(gen);
            layer.Ei.v[d] = w * layer.Ei.v[d] + 
                           c1 * r1 * ((layer.Q[d] + gbest[d]) / 2.0 - layer.Ei.x[d]);
            
            //Velocity clamping
            layer.Ei.v[d] = std::max(-v_max, std::min(v_max, layer.Ei.v[d]));
            
            //Position update
            layer.Ei.x[d] += layer.Ei.v[d];
            
            //Boundary handling
            if(layer.Ei.x[d] < x_min) {
                layer.Ei.x[d] = x_min;
                layer.Ei.v[d] = 0;
            }
            if(layer.Ei.x[d] > x_max) {
                layer.Ei.x[d] = x_max;
                layer.Ei.v[d] = 0;
            }
        }
    }
    
    void run() {
        initialize();
        
        while(FEs < max_FEs) {
            int M_Er, M_Ei;
            abs.calculateThresholds(FEs, max_FEs, M_Er, M_Ei);
            
            for(int n = 0; n < N; ++n) {
                if(FEs >= max_FEs) break;
                
                Layer& layer = layers[n];
                int action = abs.selectParticle(layer, M_Er, M_Ei);
                
                //Action 0: Reconstruct Q
                if(action == 0) {
                    layer.Er.alpha = 0;
                    layer.Ei.alpha = 0;
                    layer.beta = 0;
                    constructQ(n);
                    action = abs.selectParticle(layer, M_Er, M_Ei);
                }
                
                //Action 1: Update Er particle
                if(action == 1) {
                    updateErChannel(n);
                    double fit = objective(layer.Er.x);
                    FEs++;
                    
                    //R&P box for Er particle
                    if(fit >= layer.L_fit) {
                        layer.Er.alpha = layer.Er.alpha + 1;
                    } else {
                        layer.Er.alpha = 0;
                        layer.beta = layer.beta + 1;
                        layer.L = layer.Er.x;
                        layer.L_fit = fit;
                        
                        if(fit < gbest_fit) {
                            gbest_fit = fit;
                            gbest = layer.L;
                        }
                    }
                }
                //Action 2: Update Ei particle
                else if(action == 2) {
                    updateEiChannel(n);
                    double fit = objective(layer.Ei.x);
                    FEs++;
                    
                    //R&P box for Ei particle
                    if(fit >= layer.L_fit) {
                        layer.Ei.alpha = layer.Ei.alpha + 1;
                    } else {
                        //If only L is updated, keep alpha unchanged
                        //If G is updated, reset alpha to 0
                        layer.L = layer.Ei.x;
                        layer.L_fit = fit;
                        
                        if(fit < gbest_fit) {
                            gbest_fit = fit;
                            gbest = layer.L;
                            layer.Ei.alpha = 0;  //Reset when G is updated
                        }
                        //else: alpha unchanged (reward one more time)
                    }
                }
            }
            
            //Progress output every 100*N evaluations
            if(FEs % (100 * N) == 0) {
                std::cout << "FEs: " << FEs << " Best Fit: " << gbest_fit << std::endl;
            }
        }
    }
    
    double getGBestFitness() { return gbest_fit; }
    std::vector<double> getGBest() { return gbest; }
};