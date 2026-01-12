//cec2017/cec2017_wrapper.h
#pragma once
#include <vector>

//Forward declaration - check the actual function name in CEC 2017 code
extern "C" {
    void cec17_test_func(double*, double*, int, int, int);
}

class CEC2017Benchmark {
private:
    int func_num;
    int dim;
    
public:
    CEC2017Benchmark(int function_number, int dimension) 
        : func_num(function_number), dim(dimension) {}
    
    double evaluate(const std::vector<double>& x) {
        double* x_arr = new double[dim];
        for(int i = 0; i < dim; i++) {
            x_arr[i] = x[i];
        }
        
        double f[1];
        cec17_test_func(x_arr, f, dim, 1, func_num);
        
        double fitness = f[0];
        delete[] x_arr;
        return fitness;
    }
    
    static void getBounds(int func_num, double& lb, double& ub) {
        //Most CEC 2017 functions use [-100, 100]
        lb = -100.0;
        ub = 100.0;
    }
    
    static double getOptimalValue(int func_num) {
        //CEC 2017 optimal values
        //F1=100, F3-F30 have different values
        if (func_num == 1) return 100.0;
        else if (func_num >= 3 && func_num <= 9) return 300.0 + (func_num - 3) * 100.0;
        else if (func_num >= 10 && func_num <= 19) return 1000.0 + (func_num - 10) * 100.0;
        else if (func_num >= 20 && func_num <= 29) return 2000.0 + (func_num - 20) * 100.0;
        else if (func_num == 30) return 3000.0;
        return 0.0;
    }
};