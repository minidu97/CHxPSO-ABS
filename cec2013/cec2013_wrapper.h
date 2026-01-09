// cec2013_wrapper.h
#pragma once
#include <vector>

// Forward declaration - defined in test_func.cpp
extern "C" {
    void test_func(double*, double*, int, int, int);
}

class CEC2013Benchmark {
private:
    int func_num;
    int dim;
    
public:
    CEC2013Benchmark(int function_number, int dimension) 
        : func_num(function_number), dim(dimension) {}
    
    double evaluate(const std::vector<double>& x) {
        double* x_arr = new double[dim];
        for(int i = 0; i < dim; i++) {
            x_arr[i] = x[i];
        }
        
        double f[1];
        test_func(x_arr, f, dim, 1, func_num);
        
        double fitness = f[0];
        delete[] x_arr;
        return fitness;
    }
    
    static void getBounds(int func_num, double& lb, double& ub) {
        if (func_num >= 1 && func_num <= 5) {
            lb = -100.0; ub = 100.0;
        } else if (func_num >= 6 && func_num <= 20) {
            lb = -5.0; ub = 5.0;
        } else {
            lb = -32.0; ub = 32.0;
        }
    }
};