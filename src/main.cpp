#include "chppso_abs.h"
#include "chclpso_abs.h"
#include "benchmarks.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>
#include <unistd.h>
#include <sstream>

//Simple statistics calculation
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

//Basic Benchmark - All Dimensions
void runBasicTests(const std::vector<int>& dimensions, int N, int M, int num_runs) {
    std::cout << "\n=== Running Basic Benchmark Tests for All Dimensions ===" << std::endl;

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

    std::ofstream outfile("results_basic_all_dimensions.txt");
    outfile << std::fixed << std::setprecision(6);
    outfile << "=====================================================" << std::endl;
    outfile << "Basic Benchmark Results - CHCLPSO-ABS (All Raw Values)" << std::endl;
    outfile << "All Dimensions: ";
    for(int d : dimensions) outfile << d << "D ";
    outfile << std::endl;
    outfile << "Runs per test: " << num_runs << std::endl;
    outfile << "=====================================================" << std::endl;

    for(const auto& test : test_functions) {
        outfile << "\n\n========== Function: " << test.name << " ==========" << std::endl;
        
        //Header row with Run numbers
        outfile << "Dim\t";
        for(int run = 1; run <= num_runs; run++) {
            outfile << "Run" << run << "\t\t";
        }
        outfile << "Mean\t\tStd" << std::endl;
        outfile << "-----------------------------------------------------------" << std::endl;
        
        for(int dim : dimensions) {
            std::cout << "\n[" << test.name << " - " << dim << "D] Running..." << std::endl;
            
            int max_FEs = 10000 * dim;
            std::vector<double> results;

            for(int run = 0; run < num_runs; run++) {
                CHCLPSO_ABS alg(N, dim, max_FEs, M);
                alg.setObjective(test.func);
                alg.setBounds(test.lb, test.ub);
                alg.run();

                double fitness = alg.getGBestFitness();
                results.push_back(fitness);

                if((run + 1) % 10 == 0 || run == num_runs - 1) {
                    std::cout << "  Completed " << (run + 1) << "/" << num_runs << " runs" << std::endl;
                }
            }

            double mean, std_dev, best, worst;
            calculateStats(results, mean, std_dev, best, worst);

            std::cout << "  " << dim << "D: Mean=" << mean << std::endl;
            
            //Write dimension and all raw values
            outfile << dim << "D\t";
            for(double val : results) {
                outfile << val << "\t";
            }
            //Add mean and std at the end
            outfile << mean << "\t" << std_dev << std::endl;
        }
    }

    outfile.close();
    std::cout << "\n✓ Basic test results saved to results_basic_all_dimensions.txt" << std::endl;
}

//CEC 2013 Benchmark - All Dimensions
#ifdef USE_CEC2013
void runCEC2013Tests(const std::vector<int>& dimensions, int N, int M, int num_runs) {
    std::cout << "\n=== Running CEC 2013 Tests for All Dimensions ===" << std::endl;
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    chdir("cec2013");
    std::cout << "Working directory changed to cec2013/" << std::endl;

    std::ofstream outfile("../results_cec2013_all_dimensions.txt");
    outfile << std::fixed << std::setprecision(6);
    outfile << "=====================================================" << std::endl;
    outfile << "CEC 2013 Results - CHCLPSO-ABS (All Raw Values)" << std::endl;
    outfile << "All Dimensions: ";
    for(int d : dimensions) outfile << d << "D ";
    outfile << std::endl;
    outfile << "Runs per test: " << num_runs << std::endl;
    outfile << "=====================================================" << std::endl;

    //Test all 28 CEC 2013 functions
    for(int func = 1; func <= 28; func++) {
        outfile << "\n\n========== F" << func << " ==========" << std::endl;
        
        //Header row with Run numbers
        outfile << "Dim\t";
        for(int run = 1; run <= num_runs; run++) {
            outfile << "Run" << run << "\t\t";
        }
        outfile << "Mean\t\tStd\t\tError" << std::endl;
        outfile << "-----------------------------------------------------------------------" << std::endl;
        
        std::cout << "\n[F" << func << "] Testing across all dimensions..." << std::endl;
        
        for(int dim : dimensions) {
            std::cout << "  " << dim << "D: ";
            
            int max_FEs = 10000 * dim;
            CEC2013Benchmark benchmark(func, dim);
            std::vector<double> results;
            
            double lb, ub;
            CEC2013Benchmark::getBounds(func, lb, ub);
            
            for(int run = 0; run < num_runs; run++) {
                CHCLPSO_ABS alg(N, dim, max_FEs, M);
                alg.setBounds(lb, ub);
                
                alg.setObjective([&benchmark](const std::vector<double>& x) {
                    return benchmark.evaluate(x);
                });
                
                alg.run();
                results.push_back(alg.getGBestFitness());
                
                if((run + 1) % 10 == 0 || run == num_runs - 1) {
                    std::cout << ".";
                    std::cout.flush();
                }
            }
            
            double mean, std_dev, best, worst;
            calculateStats(results, mean, std_dev, best, worst);
            
            double optimal = CEC2013Benchmark::getOptimalValue(func);
            double error = mean - optimal;
            
            std::cout << " Mean=" << mean << ", Error=" << error << std::endl;
            
            //Write dimension and all raw values
            outfile << dim << "D\t";
            for(double val : results) {
                outfile << val << "\t";
            }
            //Add mean, std, and error at the end
            outfile << mean << "\t" << std_dev << "\t" << error << std::endl;
        }
    }
    
    outfile.close();
    chdir(original_dir);
    std::cout << "\n✓ CEC 2013 results saved to results_cec2013_all_dimensions.txt" << std::endl;
}
#endif

//CEC 2017 Benchmark - All Dimensions
#ifdef USE_CEC2017
void runCEC2017Tests(const std::vector<int>& dimensions, int N, int M, int num_runs) {
    std::cout << "\n=== Running CEC 2017 Tests for All Dimensions ===" << std::endl;
    
    char original_dir[1024];
    getcwd(original_dir, sizeof(original_dir));
    chdir("cec2017");
    std::cout << "Working directory changed to cec2017/" << std::endl;

    std::ofstream outfile("../results_cec2017_all_dimensions.txt");
    outfile << std::fixed << std::setprecision(6);
    outfile << "=====================================================" << std::endl;
    outfile << "CEC 2017 Results - CHCLPSO-ABS (All Raw Values)" << std::endl;
    outfile << "All Dimensions: ";
    for(int d : dimensions) outfile << d << "D ";
    outfile << std::endl;
    outfile << "Runs per test: " << num_runs << std::endl;
    outfile << "=====================================================" << std::endl;

    std::vector<int> functions = {1};
    for(int i = 3; i <= 30; i++) functions.push_back(i);

    for(int func : functions) {
        outfile << "\n\n========== F" << func << " ==========" << std::endl;
        
        //Header row with Run numbers
        outfile << "Dim\t";
        for(int run = 1; run <= num_runs; run++) {
            outfile << "Run" << run << "\t\t";
        }
        outfile << "Mean\t\tStd\t\tError" << std::endl;
        outfile << "-----------------------------------------------------------------------" << std::endl;
        
        std::cout << "\n[F" << func << "] Testing across all dimensions..." << std::endl;
        
        for(int dim : dimensions) {
            std::cout << "  " << dim << "D: ";
            
            int max_FEs = 10000 * dim;
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
                results.push_back(alg.getGBestFitness());
                
                if((run + 1) % 10 == 0 || run == num_runs - 1) {
                    std::cout << ".";
                    std::cout.flush();
                }
            }
            
            double mean, std_dev, best, worst;
            calculateStats(results, mean, std_dev, best, worst);
            
            double optimal = CEC2017Benchmark::getOptimalValue(func);
            double error = mean - optimal;
            
            std::cout << " Mean=" << mean << ", Error=" << error << std::endl;
            
            //Write dimension and all raw values
            outfile << dim << "D\t";
            for(double val : results) {
                outfile << val << "\t";
            }
            //Add mean, std, and error at the end
            outfile << mean << "\t" << std_dev << "\t" << error << std::endl;
        }
    }
    
    outfile.close();
    chdir(original_dir);
    std::cout << "\n✓ CEC 2017 results saved to results_cec2017_all_dimensions.txt" << std::endl;
}
#endif

//Usage
void printUsage() {
    std::cout << "\n=== CHxPSO-ABS Usage ===" << std::endl;
    std::cout << "\n./chxpso           - Run basic benchmark functions (all dimensions)" << std::endl;
    std::cout << "./chxpso cec13     - Run CEC 2013 benchmark (all dimensions)" << std::endl;
    std::cout << "./chxpso cec17     - Run CEC 2017 benchmark (all dimensions)" << std::endl;
    std::cout << "./chxpso all       - Run all available benchmarks (all dimensions)" << std::endl;
    std::cout << "\nNote: Tests will run on 10D, 30D, 50D, and 100D" << std::endl;
    std::cout << "      Results saved in separate files with all dimensions" << std::endl;
    std::cout << std::endl;
}

//Main
int main(int argc, char* argv[]) {
    //Experimental parameters (following the paper)
    std::vector<int> dimensions = {10, 30, 50, 100};  // All test dimensions
    int N = 20;                      //Population size (number of layers)
    int M = 6;                       //Total upper threshold
    int num_runs = 3;                //Number of runs

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "=====================================================" << std::endl;
    std::cout << "CHxPSO-ABS Implementation - Multi-Dimensional Testing" << std::endl;
    std::cout << "Dimensions: ";
    for(int d : dimensions) std::cout << d << "D ";
    std::cout << std::endl;
    std::cout << "Population size (N): " << N << std::endl;
    std::cout << "M: " << M << std::endl;
    std::cout << "Number of runs per test: " << num_runs << std::endl;
    std::cout << "=====================================================" << std::endl;
    
    if(num_runs == 51) {
        std::cout << "\n  WARNING: Running 51 runs per test!" << std::endl;
        std::cout << "This will take a LONG time. Consider using 3 runs for testing." << std::endl;
        std::cout << "Press Ctrl+C to cancel, or wait 5 seconds to continue..." << std::endl;
        sleep(5);
    }

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
        runBasicTests(dimensions, N, M, num_runs);
    }
    else if (mode == "cec13" || mode == "cec2013") {
#ifdef USE_CEC2013
        runCEC2013Tests(dimensions, N, M, num_runs);
#else
        std::cout << "\n CEC 2013 not available!" << std::endl;
        std::cout << "CEC 2013 files not found during compilation" << std::endl;
        return 1;
#endif
    }
    else if (mode == "cec17" || mode == "cec2017") {
#ifdef USE_CEC2017
        runCEC2017Tests(dimensions, N, M, num_runs);
#else
        std::cout << "\n CEC 2017 not available!" << std::endl;
        std::cout << "CEC 2017 files not found during compilation" << std::endl;
        return 1;
#endif
    }
    else if (mode == "all") {
        runBasicTests(dimensions, N, M, num_runs);
#ifdef USE_CEC2013
        runCEC2013Tests(dimensions, N, M, num_runs);
#endif
#ifdef USE_CEC2017
        runCEC2017Tests(dimensions, N, M, num_runs);
#endif
    }
    else {
        std::cout << "\n Unknown option: " << argv[1] << std::endl;
        printUsage();
        return 1;
    }
    
    std::cout << "\n=====================================================" << std::endl;
    std::cout << "✓ All tests completed!" << std::endl;
    std::cout << "=====================================================" << std::endl;
    
    return 0;
}