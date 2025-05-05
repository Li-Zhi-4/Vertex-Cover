extern "C" {
#include "kissat.h"  // Include Kissat C API
}

#include <iostream>
#include <regex>
#include <algorithm>
#include <map>
#include <chrono>
#include <fstream>
#include <thread>
#include <future>

#include "formatter.hpp"

/**
 * Takes input V <int> and returns <int>
 */
int vertexCommand(std::string input) {
    std::regex kPattern(R"(\s*V\s+(-?\d+)\s*)");   
    std::smatch s;
    std::regex_match(input, s, kPattern);
    return stoi(s[1]);          
}

/**
 * Captures the two digits after p cnf <int> <int>
 */
std::array<int,2> cnfCommand(std::string input) {
    std::regex cnfPattern(R"(\s*p\s+cnf\s+(-?\d+)\s+(-?\d+)\s*)");                          // Matches p cnf <int> <int> and optional whitespaces
    std::smatch s;
    std::regex_match(input, s, cnfPattern);
    return { stoi(s[1]), stoi(s[2]) };          
}

/**
 * Inputs a line of cnf into the solver
 * ex. 1 2 3 4 0 --> into solver
 */
int clauseCommand(std::string input, kissat* solver) {
    std::regex cnfPattern(R"(-?\d+)");                                          // Matches all <int> on a line and optional whitespaces
    std::smatch s;

    std::sregex_iterator num(input.begin(), input.end(), cnfPattern);            // After validation of E command format, checks (<int>, <int>, <int>)
    std::sregex_iterator cnfEnd;

    for (std::sregex_iterator n = num; n != cnfEnd; ++n) {
        kissat_add(solver, stoi(n->str()));
    }

    return 0;
}

/**
 * Takes a list of edges input (string) and converts it to a integer form
 */
std::vector<std::array<int,2>> edgeToInt(std::string edgeString) {
    std::vector<std::array<int,2>> edgeList = {};
    std::regex edgePattern(R"(<(\d+),(\d+)>)");
    std::smatch s;

    std::sregex_iterator edges(edgeString.begin(), edgeString.end(), edgePattern);
    std::sregex_iterator edgesEnd;

    for (std::sregex_iterator edge = edges; edge != edgesEnd; ++edge) {
        std::smatch match = *edge;
        std::string x = match.str(1);
        std::string y = match.str(2);
        edgeList.push_back( {stoi(x),stoi(y)} );
    }

    return edgeList;
}

/**
 * Takes input V <int> and E {edgeList} and vertex cover size (k) and outputs vertex cover clauses in cnf
 */
std::vector<std::string> cnfBuilder(int n, std::vector<std::array<int,2>> edgeList, int k) {
    std::vector<std::string> cnf = {};                      // Storage for cnf clauses
    std::vector<std::vector<int>> literals = {};            // Storage for literals

    cnf.push_back("p cnf " + std::to_string(n) + " ");      // Adding first line of cnf

    // Create literals
    for (int i = 1; i <= n*k; i += n) {
        std::vector<int> litStorage = {};
        for ( int lit = i; lit < i+n; lit++) {
            litStorage.push_back(lit);
        }
        literals.push_back(litStorage);
    }

    // Clause 1
    for (int i = 0; i < literals.size(); i++) {
        std::string clauseOne = "";
        for (int j = 0; j < literals[i].size(); j++) {
            clauseOne.append(std::to_string(literals[i][j]) + " ");
        }
        clauseOne.append("0");
        cnf.push_back(clauseOne);
    }

    // Clause 2
    for (int m = 0; m < n; m++) {
        for (int q = 0; q < k; q++) {
            for (int p = 0; p < q; p++) {
                cnf.push_back( std::to_string(-literals[p][m]) + " " + std::to_string(-literals[q][m]) + " 0" );
            }
        }
    }

    // Clause 3
    for (int m = 0; m < k; m++) {
        for (int q = 0; q < n; q++) {
            for (int p = 0; p < q; p++) {
                cnf.push_back( std::to_string(-literals[m][q]) + " " + std::to_string(-literals[m][p]) + " 0" );
            }
        }
    }

    // Clause 4
    for (int vertex = 0; vertex < edgeList.size(); vertex++) {
        std::string clause = "";
        for (int i = 0; i < k; i++) {
            clause.append(std::to_string(literals[i][edgeList[vertex][0]-1]) + " ");
            clause.append(std::to_string(literals[i][edgeList[vertex][1]-1]) + " ");
        }
        clause += "0";
        cnf.push_back(clause);
    }

    cnf[0] += std::to_string(cnf.size()-1);
    return cnf;
}

/**
 * Given clauses in cnf form, returns the vertex cover
 */
std::vector<int> cnfSolver(int n, std::vector<std::array<int,2>> edgeList, int k) {
    kissat *solver = kissat_init();  // Initialize solver
    std::vector<int> cover = {};
    std::vector<std::string> clauses = cnfBuilder(n, edgeList, k);
    
    for (int i = 1; i < clauses.size(); i++) {
        clauseCommand(clauses[i],solver);
        //std::cout << clauses[i] << std::endl;
    }

    int result = kissat_solve(solver);  // Solve
    
    if (result == 10) {  // 10 = SAT
        //std::cout << "SATISFIABLE\n";  
        
        for (int i = 0; i < k; i++) {
            for (int j = 1; j <= n; j++) {
                int value = kissat_value(solver, j + n*i);
                if (value > 0) {
                    cover.push_back(j);
                }
            }
        }
        std::sort(cover.begin(), cover.end());

    } else if (result == 20) {  // 20 = UNSAT
        //std::cout << "UNSATISFIABLE\n";
    }

    kissat_release(solver);  // Free solver memory

    return cover;
}

/**
 * Prints vertexCover
 */
std::string printVertexCover(std::vector<int> vertexCover) {
    std::string vc = "";
    std::sort(vertexCover.begin(), vertexCover.end());

    for (int i = 0; i < vertexCover.size(); i++) {
        vc.append(std::to_string(vertexCover[i]) + " ");
    }
    vc.append("(" + std::to_string(vertexCover.size()) + ")");          // NOTE: PRINTING FOR ASSIGNEMENT
    //vc.append("," + std::to_string(vertexCover.size()));              // NOTE: PRINTINT FOR CSV
    return vc;
}

// ================= Greedy Solver Functions Below =================

/**
 * Creates an edge map given the max number of vertices (n) and a list of edges (int)
 */
std::map<int,std::vector<int>> createEdgeMap(std::vector<std::array<int,2>> edgeList, int n) {
    std::map<int,std::vector<int>> edgeMap = {};

    for (const auto& vertices : edgeList) {                             // Loops through every edge (x,y)
        if (edgeMap.find(vertices[0]) != edgeMap.end()) {               // Checks if x is a key in the map first
            edgeMap[vertices[0]].push_back(vertices[1]);
        } else if (edgeMap.find(vertices[0]) == edgeMap.end()) {  
            edgeMap[vertices[0]] = {vertices[1]};
        }

        if (edgeMap.find(vertices[1]) != edgeMap.end()) {               // Checks if y is a key in the map first (for undirected)
            edgeMap[vertices[1]].push_back(vertices[0]);
        } else if (edgeMap.find(vertices[1]) == edgeMap.end()) {  
            edgeMap[vertices[1]] = {vertices[0]};
        }
    }

    return edgeMap;
}

/**
 * Creates vertex cover for greedysolver 1 algorithm
 */
std::vector<int> greedySolver1(std::vector<std::array<int,2>> edgeList, int n) {
    std::map<int,std::vector<int>> edgesDict = createEdgeMap(edgeList, n);
    std::vector<int> vertexCover = {};
    std::vector<int> removeVertex;                          // Storage for vertices to be removed from edgesDict
    int currentVertex;                                      // Storage for vertex with highest number of adjacent vertice
    int vertexSize;                                         // Storage for number of adjacent vertice (comparison)

    while (edgesDict.size() > 0) {
        currentVertex = 0;
        vertexSize = 0;
        removeVertex = {};

        for (const auto& vertexKey : edgesDict) {           // Searching for vertex with most adjacent vertices
            if (vertexKey.second.size() > vertexSize) { 
                currentVertex = vertexKey.first;
                vertexSize = vertexKey.second.size();
            }
        }

        vertexCover.push_back(currentVertex);               // Recording vertex cover
        removeVertex.push_back(currentVertex);              // Recording vertex to be removed from edgesDict

        for (const auto& i : edgesDict) {                   // Removing vertex from all other adjacency lists
            edgesDict[i.first].erase(std::remove(edgesDict[i.first].begin(), edgesDict[i.first].end(), currentVertex), edgesDict[i.first].end()); 
            if (i.second.size() == 0) {                     // Removing all vertex with empty adjacency lists
                removeVertex.push_back(i.first);
            }
        }

        for (const auto& v : removeVertex) {                // Deleting vertices to be removed from edgesDict
            edgesDict.erase(v);
        }

    } 
    
    return vertexCover;
}

/**
 * Creates vertex cover for greedysolver 2 algorithm
 */
std::vector<int> greedySolver2(std::vector<std::array<int,2>> edgeList, int n) {
    std::map<int,std::vector<int>> edgesDict = createEdgeMap(edgeList, n);
    std::vector<int> vertexCover = {};
    std::vector<std::array<int,2>> removeEdge;                          // Storage for vertices to be removed from edgesDict
    std::vector<int> removeVertex;                                      // Storage for vertices to be removed from edgesDict
    int x,y, px, py;                                                    // Storage for vertex with highest number of adjacent vertice
    int sum, highestSum;                                                // Storage for number of adjacent vertice (comparison)

    while (edgeList.size() > 0) {
        px = 0;
        py = 0;
        highestSum = 0;
        removeVertex = {};
        removeEdge = {};

        for (int i = 0; i < edgeList.size(); i++) {                     // Searching for edge with most adjacent vertices
            //std::cout << "(" << edgeList[i][0] << ", " << edgeList[i][1] << ")" << std::endl;
            sum = 0;
            x = edgeList[i][0];
            y = edgeList[i][1];
            for (const auto& vertexKey : edgesDict) {                   // Sums number of adjacent vertice in edge
                if (vertexKey.first == x || vertexKey.first == y) {
                    sum += vertexKey.second.size();
                }
            }
            //std::cout << "(" << x << "," << y << ") has " << sum << " adjacent neighbors" << std::endl;
            if (sum > highestSum) {                                     // If vertex pair has most adjacent vertices, record it
                highestSum = sum;
                px = x;
                py = y;
            }
        }

        vertexCover.push_back(px);  
        vertexCover.push_back(py);
        removeVertex.push_back(px);              
        removeVertex.push_back(py);              
        //std::cout << "Added to vertex cover: " << px << " " << py << std::endl;

        for (const auto& i : edgesDict) {                   // Removing vertex from all other adjacency lists
            edgesDict[i.first].erase(std::remove(edgesDict[i.first].begin(), edgesDict[i.first].end(), px), edgesDict[i.first].end()); 
            edgesDict[i.first].erase(std::remove(edgesDict[i.first].begin(), edgesDict[i.first].end(), py), edgesDict[i.first].end()); 
            if (i.second.size() == 0) {                     // Removing all vertex with empty adjacency lists
                removeVertex.push_back(i.first);
            }
        }

        for (const auto& v : removeVertex) {                // Deleting vertices to be removed from edgesDict
            edgesDict.erase(v);
        }

        for (int i = 0; i < edgeList.size(); i++) {         // Records all edges in edgeList that have vertice in vertexCover
            if (px == edgeList[i][0] || px == edgeList[i][1] || py == edgeList[i][0] || py == edgeList[i][1]) {
                removeEdge.push_back({edgeList[i][0],edgeList[i][1]});
                //std::cout << "Removed: (" << edgeList[i][0]+1 << "," << edgeList[i][1]+1 << ")" << std::endl;
            }
        }
        
        for (const auto& edge : removeEdge) {               // Removes edges in edgeList that have been covered
            auto index = std::find(edgeList.begin(), edgeList.end(), edge);
            edgeList.erase(index);
            //std::cout << "Removed: " << edge[0] << "," << edge[1] << std::endl;
        }
        
    }
    
    return vertexCover;
}

// ================= Write to CSV Below =================

/**
 * For recording data purposes
 */
void writeToFile(const std::string& data) {
    std::ofstream outputToFile("Output/random_file.txt", std::ios::app);
    if (outputToFile.is_open()) {
        outputToFile << data;
        outputToFile.close();
    } else {
        //std::cout << "Error" << std::endl;
    }
}

int main() {
    int v;
    //int k = 1;
    std::string command;
    std::vector<std::array<int,2>> edgeList = {};
    std::vector<int> vertexCover = {};

    std::string line;
    while (!(getline(std::cin, line)).eof()) {

        if (line[0] == 'V') {               // Checks for V commands
            v = vertexCommand(line);
            writeToFile(std::to_string(v) + "\n");
        } else if (line[0] == 'E') {        // Checks for E commands
            edgeList = edgeToInt(line);

            std::vector<int> kUpperBound = greedySolver1(edgeList, v);
            std::vector<int> minVertexCover = {};
            std::vector<int> vertexCover = {};
            int maxK = kUpperBound.size();

            auto start = std::chrono::high_resolution_clock::now();     // Solves greedysolver 1 and records data
            writeToFile("VC-GREEDY-1: " + printVertexCover(greedySolver1(edgeList, v)) + "\n");
            auto end = std::chrono::high_resolution_clock::now();
            writeToFile("Execution time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) + "\n");

            start = std::chrono::high_resolution_clock::now();          // Solves greedysolver 2 and records data
            writeToFile("VC-GREEDY-2: " + printVertexCover(greedySolver2(edgeList, v)) + "\n");
            end = std::chrono::high_resolution_clock::now();
            writeToFile("Execution time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) + "\n");

            while (maxK > 0) {      // Loops through different values of k to find optimal solution
                start = std::chrono::high_resolution_clock::now();
                auto test = async(std::launch::async, cnfSolver, v, edgeList, maxK);            // Launches a timer thread
                if (test.wait_for(std::chrono::minutes(10)) == std::future_status::ready) {     // Waits 10 minutes for solver
                    vertexCover = test.get();
                } else {            // If timer runs out, records previous iteration of vertex cover
                    //std::cout << "TIMEOUT!" << std::endl;
                    writeToFile("VC (non-optimal): " + printVertexCover(minVertexCover) + "\n");
                    end = std::chrono::high_resolution_clock::now();
                    writeToFile("Execution time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) + "\n\n");
                    //exit(1);
                    break;
                }

                if (vertexCover.size() == 0) {      // If vertex cover is 0, unsatisfiable, return previous iteration of vertex cover
                    std::cout << "VC-EXACT: " << printVertexCover(minVertexCover) << std::endl;
                    writeToFile("VC-EXACT: " + printVertexCover(minVertexCover) + "\n");
                    end = std::chrono::high_resolution_clock::now();
                    writeToFile("Execution time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) + "\n\n");
                    break;
                } else {                            // Else, vertex cover is satisfiable, record data and continue to check next iteration of k
                    minVertexCover = vertexCover;
                }
                --maxK;
            }

            std::cout << "VC-GREEDY-1: " << printVertexCover(greedySolver1(edgeList, v)) << std::endl;
            std::cout << "VC-GREEDY-2: " << printVertexCover(greedySolver2(edgeList, v)) << std::endl;
        } 

    }

    return 0;
}
