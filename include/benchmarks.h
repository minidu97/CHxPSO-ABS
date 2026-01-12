#pragma once
#include <vector>
#include <cmath>

//Basic benchmark functions
inline double sphere(const std::vector<double>& x) {
    double sum = 0.0;
    for(double xi : x) {
        sum += xi * xi;
    }
    return sum;
}

inline double rastrigin(const std::vector<double>& x) {
    double sum = 0.0;
    const double A = 10.0;
    const double pi = 3.14159265358979323846;
    
    for(double xi : x) {
        sum += xi * xi - A * std::cos(2 * pi * xi);
    }
    return A * x.size() + sum;
}

inline double rosenbrock(const std::vector<double>& x) {
    double sum = 0.0;
    for(size_t i = 0; i < x.size() - 1; ++i) {
        double term1 = x[i+1] - x[i] * x[i];
        double term2 = x[i] - 1.0;
        sum += 100.0 * term1 * term1 + term2 * term2;
    }
    return sum;
}

inline double ackley(const std::vector<double>& x) {
    double sum1 = 0.0;
    double sum2 = 0.0;
    const double pi = 3.14159265358979323846;
    
    for(double xi : x) {
        sum1 += xi * xi;
        sum2 += std::cos(2 * pi * xi);
    }
    
    double n = static_cast<double>(x.size());
    return -20.0 * std::exp(-0.2 * std::sqrt(sum1 / n)) 
           - std::exp(sum2 / n) + 20.0 + std::exp(1.0);
}

//CEC 2013 Wrapper - include the wrapper file, not the implementation
#ifdef USE_CEC2013
#include "../cec2013/cec2013_wrapper.h"
#endif

//CEC 2017 Wrapper - include the wrapper file, not the implementation
#ifdef USE_CEC2017
#include "../cec2017/cec2017_wrapper.h"
#endif