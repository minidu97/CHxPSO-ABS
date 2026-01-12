#include "chppso_abs.h"
#include "chclpso_abs.h"
#include "benchmarks.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>
#include <unistd.h> 

//Simple statistics calculation for the first part of the paper
void calculateStats(const std::vector<double>& results, double& mean, double& std_dev, double& best, double& worst) {
    mean = 0.0;
    best = results[0];
    worst = results[0];

    for(double r : results) {
        mean += r;
        if(r < best) best = r;
        if(r > worst) worst = r;
    }
    mean /= results.size();

    std_dev = 0.0;
    for(double r : results) {
        std_dev += (r - mean) * (r - mean);
    }
    std_dev = std::sqrt(std_dev / results.size());
}

//Basic benchmark code 
void runBasicTests(int dim, int N, int max_FEs, int M, int num_runs) {
    std::cout << "\n=== Running Basic Benchmark Tests ===" << std::endl;

    struct TestFunction {
        std::string name;
        std::function<double(const std::vector<double>&)> func;
        double lb, ub;
    };

    std::vector<TestFunction> test_functions = {
        {"Sphere", sphere, -100.0, 100.0},
        {"Rastrigin", rastrigin, -5.12, 5.12},
        {"Rosenbrock", rosenbrock, -30.0, 30.0},
        {"Ackley", ackley, -32.0, 32.0}
    };

    std::ofstream outfile("results_basic.txt");
    outfile << std::fixed << std::setprecision(6);

    std::cout << "\n=== Testing CHCLPSO-ABS ===" << std::endl;
    outfile << "=== CHCLPSO-ABS Results ===" << std::endl;

    for(const auto& test : test_functions) {
        std::cout << "\nFunction: " << test.name << std::endl;
        outfile << "\nFunction: " << test.name << std::endl;

        std::vector<double> results;

        for(int run = 0; run < num_runs; run++) {
            CHCLPSO_ABS alg(N, dim, max_FEs, M);
            alg.setObjective(test.func);
            alg.setBounds(test.lb, test.ub);
            alg.run();

            double fitness = alg.getGBestFitness();
            results.push_back(fitness);

            std::cout << "  Run " << (run+1) << ": " << fitness << std::endl;
        }

        double mean, std_dev, best, worst;
        calculateStats(results, mean, std_dev, best, worst);

        std::cout << "  Mean: " << mean << " Std: " << std_dev
                  << " Best: " << best << " Worst: " << worst << std::endl;
        outfile << "Mean: " << mean << " Std: " << std_dev
                << " Best: " << best << " Worst: " << worst << std::endl;
    }

    outfile.close();
    std::cout << "\nBasic test results saved to results_basic.txt" << std::endl;
}

//CEC2013 Benchmark Code
#ifdef USE_CEC2013
void runCEC2013Tests(int dim, int N, int max_FEs, int M, int num_runs) {
    std::cout << "\n=== Running CEC 2013 Tests ===" << std::endl;
    
    //Change to cec2013 directory so test_func can find input_data
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    chdir("cec2013");
    std::cout << "Working directory changed to cec2013/" << std::endl;

    std::ofstream outfile("../results_cec2013.txt");
    outfile << std::fixed << std::setprecision(6);
    outfile << "CEC 2013 Results - CHCLPSO-ABS" << std::endl;
    outfile << "Dimension: " << dim << ", Runs: " << num_runs << std::endl;
    outfile << "\nFunc\tMean\t\tStd\t\tBest\t\tWorst\t\tError" << std::endl;

    //Test all 28 CEC 2013 functions
    for(int func = 1; func <= 28; func++) {
        std::cout << "\nTesting F" << func << "..." << std::endl;
        
        //Create benchmark object for this function
        CEC2013Benchmark benchmark(func, dim);
        std::vector<double> results;
        
        //Get bounds for this function
        double lb, ub;
        CEC2013Benchmark::getBounds(func, lb, ub);
        
        //Run multiple times
        for(int run = 0; run < num_runs; run++) {
            CHCLPSO_ABS alg(N, dim, max_FEs, M);
            alg.setBounds(lb, ub);
            
            //Use lambda to capture benchmark
            alg.setObjective([&benchmark](const std::vector<double>& x) {
                return benchmark.evaluate(x);
            });
            
            alg.run();
            
            double fitness = alg.getGBestFitness();
            results.push_back(fitness);
            
            std::cout << "  Run " << (run+1) << ": " << fitness << std::endl;
        }
        
        //Calculate statistics
        double mean, std_dev, best, worst;
        calculateStats(results, mean, std_dev, best, worst);
        
        //Calculate error (fitness - optimal)
        double optimal = CEC2013Benchmark::getOptimalValue(func);
        double error = mean - optimal;
        
        std::cout << "F" << func << " Mean: " << mean 
                  << " Error: " << error << std::endl;
        
        outfile << "F" << func << "\t" 
                << mean << "\t" << std_dev << "\t" 
                << best << "\t" << worst << "\t" 
                << error << std::endl;
    }
    
    outfile.close();
    
    //Revert back to the original directory
    chdir(original_dir);
    std::cout << "\nCEC 2013 results saved to results_cec2013.txt" << std::endl;
}
#endif

//CEC2017 Benchmark Code
#ifdef USE_CEC2017
void runCEC2017Tests(int dim, int N, int max_FEs, int M, int num_runs) {
    std::cout << "\n=== Running CEC 2017 Tests ===" << std::endl;
    
    //Change to cec2017 directory to find input_data
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    chdir("cec2017");
    std::cout << "Working directory changed to cec2017/" << std::endl;

    std::ofstream outfile("../results_cec2017.txt");
    outfile << std::fixed << std::setprecision(6);
    outfile << "CEC 2017 Results - CHCLPSO-ABS" << std::endl;
    outfile << "Dimension: " << dim << ", Runs: " << num_runs << std::endl;
    outfile << "\nFunc\tMean\t\tStd\t\tBest\t\tWorst\t\tError" << std::endl;

    //Test CEC 2017 functions
    std::vector<int> functions = {1};
    for(int i = 3; i <= 30; i++) functions.push_back(i);

    for(int func : functions) {
        std::cout << "\nTesting F" << func << "..." << std::endl;
        
        //Create benchmark object for this function
        CEC2017Benchmark benchmark(func, dim);
        std::vector<double> results;
        
        double lb, ub;
        CEC2017Benchmark::getBounds(func, lb, ub);
        
        for(int run = 0; run < num_runs; run++) {
            CHCLPSO_ABS alg(N, dim, max_FEs, M);
            alg.setBounds(lb, ub);
            
            alg.setObjective([&benchmark](const std::vector<double>& x) {
                return benchmark.evaluate(x);
            });
            
            alg.run();
            
            double fitness = alg.getGBestFitness();
            results.push_back(fitness);
            
            std::cout << "  Run " << (run+1) << ": " << fitness << std::endl;
        }
        
        double mean, std_dev, best, worst;
        calculateStats(results, mean, std_dev, best, worst);
        
        double optimal = CEC2017Benchmark::getOptimalValue(func);
        double error = mean - optimal;
        
        std::cout << "F" << func << " Mean: " << mean 
                  << " Error: " << error << std::endl;
        
        outfile << "F" << func << "\t" 
                << mean << "\t" << std_dev << "\t" 
                << best << "\t" << worst << "\t" 
                << error << std::endl;
    }
    
    outfile.close();
    
    //Revert back to the original directory
    chdir(original_dir);
    std::cout << "\nCEC 2017 results saved to results_cec2017.txt" << std::endl;
}
#endif

//personal ease 
void printUsage() {
    std::cout << "\n=== CHxPSO-ABS Usage ===" << std::endl;
    std::cout << "\n./chxpso           - Run basic benchmark functions" << std::endl;
    std::cout << "./chxpso cec13     - Run CEC 2013 benchmark (28 functions)" << std::endl;
    std::cout << "./chxpso cec17     - Run CEC 2017 benchmark (29 functions)" << std::endl;
    std::cout << "./chxpso all       - Run all available benchmarks" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  ./chxpso              # Quick test with basic functions" << std::endl;
    std::cout << "  ./chxpso cec13        # Test on CEC 2013" << std::endl;
    std::cout << "  ./chxpso all          # Complete benchmark suite" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    //Experimental parameters
    int dim = 10;                    //Dimension
    int N = 20;                      //Population size (number of layers)
    int max_FEs = 10000 * dim;       //Maximum function evaluations
    int M = 6;                       //Total upper threshold
    int num_runs = 51;               //Number of runs 

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "================================================" << std::endl;
    std::cout << "CHxPSO-ABS Implementation" << std::endl;
    std::cout << "Dimension: " << dim << std::endl;
    std::cout << "Population size (N): " << N << std::endl;
    std::cout << "Max FEs: " << max_FEs << std::endl;
    std::cout << "M: " << M << std::endl;
    std::cout << "Number of runs: " << num_runs << std::endl;
    std::cout << "================================================" << std::endl;

    //Parse command line arguments
    std::string mode = "basic";
    if (argc > 1) {
        mode = argv[1];
    }
    
    //Convert to lowercase for comparison
    for(auto& c : mode) c = tolower(c);
    
    if (mode == "help" || mode == "-h" || mode == "--help") {
        printUsage();
        return 0;
    }
    
    //Run based on mode
    if (mode == "basic" || argc == 1) {
        runBasicTests(dim, N, max_FEs, M, num_runs);
    }
    else if (mode == "cec13" || mode == "cec2013") {
#ifdef USE_CEC2013
        runCEC2013Tests(dim, N, max_FEs, M, num_runs);
#else
        std::cout << "\n❌ CEC 2013 not available!" << std::endl;
        std::cout << "CEC 2013 files not found during compilation" << std::endl;
        return 1;
#endif
    }
    else if (mode == "cec17" || mode == "cec2017") {
#ifdef USE_CEC2017
        runCEC2017Tests(dim, N, max_FEs, M, num_runs);
#else
        std::cout << "\n❌ CEC 2017 not available!" << std::endl;
        std::cout << "CEC 2017 files not found during compilation" << std::endl;
        return 1;
#endif
    }
    else if (mode == "all") {
        runBasicTests(dim, N, max_FEs, M, num_runs);
#ifdef USE_CEC2013
        runCEC2013Tests(dim, N, max_FEs, M, num_runs);
#endif
#ifdef USE_CEC2017
        runCEC2017Tests(dim, N, max_FEs, M, num_runs);
#endif
    }
    else {
        std::cout << "\n Wrong Option : " << argv[1] << std::endl;
        printUsage();
        return 1;
    }
    
    std::cout << "\n================================================" << std::endl;
    std::cout << "✓ All tests completed!" << std::endl;
    std::cout << "================================================" << std::endl;
    
    return 0;
}