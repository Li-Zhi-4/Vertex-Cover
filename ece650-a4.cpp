#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <chrono>

#include "matrix.hpp"
#include "formatter.hpp"

// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"

// defined std::cout
#include <iostream>
 

int main(const int argc, const char* argv[]) {
    // Test code. Replaced with your code
    std::string command;
    int size;
    bool graphExists = false;
    Matrix graph(1,1);
    std::vector<std::array<int,3>> eInput;
    std::array<int,2> sInput;
  
    // read from stdin until EOF
    std::string line;
    while (!(getline(std::cin, line)).eof()) {

        // Code to process input lines
        command = commandFormatter(line);     // Checks for N, E, or S command
        if (command == "F") {
            continue;
        }
  
        if (graphExists == false) {           // If a graph does not exist already
            if (command != "V") {
                std::cerr << "Error: Must create graph first before using E or S.\n";
                continue;
            } 
        } 
  
        if (command == "V") {                                         // If command is N  

            size = nCommand(line);
            if (size != -1) {
                Matrix temp(size,size);                                 // Resize the graph as specified
                graph = temp;
                graphExists = true;
            }
    
        } else if (command == "E") {                                  // If command is E
            
            eInput = eCommand(line);
            std::vector<std::array<int,3>> errorInput = { { -1, -1, -1 } };
            if (eInput != errorInput) {
                for (size_t i = 0; i < eInput.size(); i++) {
                    graph.set(eInput[i][0]-1,eInput[i][1]-1,eInput[i][2]);  // Set edges in graph
                }

                //graph.print();
                auto start = std::chrono::high_resolution_clock::now();
                std::cout << graph.vcExact(size) << std::endl;
                auto end = std::chrono::high_resolution_clock::now();
                //std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " µs" << std::endl;

                start = std::chrono::high_resolution_clock::now();
                std::cout << graph.greedySolver1() << std::endl;
                end = std::chrono::high_resolution_clock::now();
                //std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " µs" << std::endl;

                start = std::chrono::high_resolution_clock::now();
                std::cout << graph.greedySolver2() << std::endl;
                end = std::chrono::high_resolution_clock::now();
                //std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " µs" << std::endl;
                //std::cout << std::endl;
            }
    
        } else if (command == "S") {
            
            sInput = sCommand(line);
            std::array<int,2> errorInput = { -1, -1 };
            std::string ans;
            if (sInput != errorInput) {
                if (sInput[0] > size || sInput[1] > size) {               // Checks ints are within bounds
                    std::cerr << "Error: Input is out of bounds. Choose valid vertice.\n";
                } else {
                    graph.dijkstra(sInput[0]-1);
                    ans = graph.printPathway(sInput[0]-1,sInput[1]-1, size-1);    // testing, -1 already accounted for in function maybe.
                    std::cout << ans << "\n";
                }
            }

        } 
  
    }
}
  
 