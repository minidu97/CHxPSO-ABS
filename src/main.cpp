#include "chxpso_abs.h"
#include <iostream>
#include <cmath>

double sphereFunction(const std::vector<double>& x) {
    double sum = 0.0;
    for(double xi : x) sum += xi*xi;
    return sum;
}

int main() {
    int dim = 10;
    int swarm_size = 30;
    int max_iter = 1000;
    int M = 5;  // parameter for ABS strategy

    CHxPSO_ABS optimizer(swarm_size, dim, max_iter, M);
    optimizer.setObjective(sphereFunction);

    optimizer.run();

    std::cout << "Best fitness found: " << optimizer.getGBestFitness() << std::endl;
    return 0;
}
