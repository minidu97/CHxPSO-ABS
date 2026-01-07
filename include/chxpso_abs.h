#pragma once
#include "particle.h"
#include "abs_strategy.h"
#include <vector>
#include <functional>
#include <random>
#include <iostream>

class CHxPSO_ABS {
protected:
    int swarm_size;
    int dim;
    int max_iter;
    std::vector<Particle> swarm;
    std::vector<double> gbest;
    double gbest_fit;
    ABSStrategy abs;
    std::vector<int> active_particles;
    std::function<double(const std::vector<double>&)> objective;

    std::mt19937 gen;
    std::uniform_real_distribution<double> dis;

public:
    CHxPSO_ABS(int swarm_size_, int dim_, int max_iter_, int M_)
        : swarm_size(swarm_size_), dim(dim_), max_iter(max_iter_), abs(M_), gbest_fit(1e9), dis(0.0,1.0) {
        gen.seed(std::random_device{}());
        gbest.resize(dim);
    }

    void setObjective(std::function<double(const std::vector<double>&)> obj) {
        objective = obj;
    }

    void initialize() {
        swarm.clear();
        for(int i=0;i<swarm_size;++i){
            Particle p;
            p.x.resize(dim);
            p.v.resize(dim);
            p.pbest.resize(dim);
            p.pbest_fit = 1e9;
            p.role = (i % 2 == 0) ? ER : EI;
            for(int d=0;d<dim;++d){
                p.x[d] = dis(gen)*10 - 5;  // [-5,5]
                p.v[d] = dis(gen)*1 - 0.5; // [-0.5,0.5]
                p.pbest[d] = p.x[d];
            }
            swarm.push_back(p);
        }
    }

    void updateVelocity(int idx) {
        Particle &p = swarm[idx];
        double w = 0.7, c1 = 1.5, c2 = 1.5;
        for(int d=0;d<dim;++d){
            double r1 = dis(gen), r2 = dis(gen);
            p.v[d] = w*p.v[d] + c1*r1*(p.pbest[d]-p.x[d]) + c2*r2*(gbest[d]-p.x[d]);
        }
    }

    void updatePosition(int idx) {
        Particle &p = swarm[idx];
        for(int d=0;d<dim;++d){
            p.x[d] += p.v[d];
        }
    }

    void run() {
        initialize();
        for(int iter=0;iter<max_iter;++iter){
            abs.updateActiveSet(iter, max_iter, swarm, active_particles);
            for(int i : active_particles){
                updateVelocity(i);
                updatePosition(i);
                double fit = objective(swarm[i].x);
                if(fit < swarm[i].pbest_fit){
                    swarm[i].pbest_fit = fit;
                    swarm[i].pbest = swarm[i].x;
                }
                if(fit < gbest_fit){
                    gbest_fit = fit;
                    gbest = swarm[i].x;
                }
            }
            if(iter%100==0) std::cout << "Iter " << iter << " Best Fit: " << gbest_fit << std::endl;
        }
    }

    double getGBestFitness() { return gbest_fit; }
};
