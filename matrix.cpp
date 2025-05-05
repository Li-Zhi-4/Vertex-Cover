#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>
#include <map>
#include <chrono>

#include "matrix.hpp"
// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"


/**
 * Matrix Class
 * Enables the creation of an adjacency matrix to solve graphing problems.
 * 
 * @param newRow the size of the graph
 * @param newCol the size of the graph
 */

// Constructor and Destructor
Matrix::Matrix(int newRow, int newCol)
    : row(newRow), col(newCol), graph(newRow, std::vector<int>(newCol)), distance(newRow, INT_MAX), parent(newRow, -1) {}

Matrix::~Matrix() {}

/** 
 * Returns the weight between two vertex.
 * 
 * @param r the vertex 1 (v1)
 * @param c the vertex 2 (v2)
 * @return the weight between v1 and v2.
 */
int Matrix::get(int r, int c) {
    return graph[r][c];
}

/** 
 * Sets an edge in a matrix with a specific weight.
 * 
 * @param r the vertex 1 (v1)
 * @param c the vertex 2 (v2)
 * @param w the weight between v1 and v2.
 */
void Matrix::set(int r, int c, int w) {   
    // Checks out of bounds error
    if (r < 0 || c < 0 || w <= 0 || r >= row || c >= col) {
        std::cerr << "Error: Edge (" << r << ", " << c << ") is out of bounds.\n";
        return;
    }

    if (get(r,c) != 0) {                // Checks if edge exists

        if (get(r,c) == w) {            // Checks if weight is the same value, don't update
            std::cerr << "Error: Edge with same weight already exists. Did not update graph.\n";
            return;
        }

        std::cerr << "Error: Edge already exists. Updating weight.\n";
    }
    //std::cout << "Adding: (" << r+1 << ", " << c+1 << ")" << std::endl;
    edgeList.push_back({r, c});
    graph[r][c] = w;
    graph[c][r] = w;
}

/** 
 * Resizes the matrix.
 * 
 * @param r the vertex 1 (v1)
 * @param c the vertex 2 (v2)
 */
void Matrix::resize(int r, int c) {
    row = r;
    col = c;
}

/** 
 * Prints the matrix graph and parent/distance vectors (for testing purposes).
 */
void Matrix::print() {
    // Graph
    for (int r = 0; r < row; r++) {
        for (int c = 0; c < col; c++) {
            std::cout << get(r,c) << " ";
        }
        std::cout << std::endl;
    }

    // Parent and Distance
    std::cout << "Parent Array: ";
    for (int i = 0; i < row; i++) {
        std::cout << parent[i] << ", ";
    }
    std::cout << "\n";
    std::cout << "Distance Array: ";
    for (int i = 0; i < row; i++) {
        std::cout << distance[i] << ", ";
    }
    std::cout << "\n";
}

/**
 * Solves dijkstra's algorithm and updates a parent and distance vectors with respect to the source.
 * 
 * @param source the source vertex
 */
void Matrix::dijkstra(int source) {
    // Reset parent and distance vectors
    std::fill(parent.begin(), parent.end(), -1);
    std::fill(distance.begin(), distance.end(), INT_MAX);
    std::vector<bool> visited(row, false);
    distance[source] = 0;

    for (int r = 0; r < row; r++) {
        int minDist = INT_MAX;
        int u = -1;

        for (int c = 0; c < col; c++) {                     // Looping through distance vector
            if (distance[c] < minDist && !visited[c]) {
                minDist = distance[c];
                u = c;                                      // u is the node with the smallest distance
            }
        }

        if (u == -1) {                                      // Otherwise no smallest node
            break;
        }

        visited[u] = true;                                  // Update visited to prevent rechecking nodes

        for (int v = 0; v < row; v++) {                     // Check all the neighboring nodes of u (i.e., check all of v)
            if (!visited[v] && graph[u][v] != 0 && distance[u] + graph[u][v] < distance[v]) {
                distance[v] = distance[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
}

/**
 * Prints the pathway from the source to the target and the total weight.
 * 
 * @param source the source vertex
 * @param target the target vertex
 * @param size the size of the graph
 */
std::string Matrix::printPathway(int source, int target, int size) {    
    std::vector<int> pathway;
    std::string path;
    int update = target;
    int count = 0;

    pathway.push_back(update+1);                            // Adds target vertex to pathway
    if (distance[target] == INT_MAX) {                      // Checks if pathway exists
        //std::cout << "No pathway!\n";
        return "No path exists.";
    } else {
        while (count < size) {
            update = parent[update];                        // Chains through parent vector to create pathway
            pathway.push_back(update+1);
            if (update == source) {                         // Checks if source has been reached
                break;
            }
            ++count;
        }
    }
 
    for (int i = pathway.size()-1; i >= 0; i--) {
        path.append(std::to_string(pathway[i]));            // Reverses the pathway
        if (i != 0) {
            path.append("-");
        }
    }

    path.append(" ");
    path.append(std::to_string(distance[target]));
    //std::cout << path << " " << distance[target] << "\n";   // Prints pathway
    return path;
}

/**
 * Takes the size of the number of vertices and returns the minimum vertex cover
 */
std::string Matrix::vcExact(int size) {
    std::string cover, minCover;
    std::vector<int> vertexCover;
    int n = size;
    //int count = 0;

    //for (int k = 1; k <= 9; k++) {
    for (int k = size; k > 0; k--) {
        n = size;
        cover = "VC-EXACT: ";
        //count = 0; // For tracking size of vertex cover
        vertexCover = {};
        std::vector<std::vector<Minisat::Lit>> x(k, std::vector<Minisat::Lit>(n));
        std::vector<Minisat::vec<Minisat::Lit>> clauseOne(k);
        std::vector<Minisat::vec<Minisat::Lit>> clauseFour(edgeList.size());

        // -- allocate on the heap so that we can reset later if needed
        std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());

        for (int i = 0; i < k; i++) {   // row
            for (int j = 0; j < n; j++) {   // col
                x[i][j] = Minisat::mkLit(solver->newVar());     // x[i][j] where i is position and j is vertex (backwards from notes)
            }
        }

        //std::cout << "Clause 1: \n";
        // CLAUSE 1: One vertex in each position
        for (int i = 0; i < k; i++) {
            //std::cout << "k = " << i+1 << ": ";
            for (int j = 0; j < n; j++) {
                clauseOne[i].push(x[i][j]);
                //std::cout << "x[" << i+1 << "][" << j+1 << "]" << " ";
            }
            solver->addClause(clauseOne[i]);
            //std::cout << std::endl;
        }  

        // CLAUSE 2: Vertex cannot be in both position p and q
        //std::cout << "Clause 2: \n";
        for (int m = 0; m < n; m++) {
            for (int q = 0; q < k; q++) {
                for (int p = q+1; p < k; p++) {
                    solver->addClause(~x[p][m], ~x[q][m]);
                    //std::cout << "Clause: ~x[" << p+1 << "][" << m+1 << "], ~x[" << q+1 << "][" << m+1 << "]" << std::endl;
                }
            }
        }  
  
        // CLAUSE 3: Position m can only have one vertex, p or q
        //std::cout << "Clause 3: \n";
        for (int m = 0; m < k; m++) {
            for (int p = 0; p < n; p++) {
                for (int q = p+1; q < n; q++) {
                    solver->addClause(~x[m][p], ~x[m][q]);
                    //std::cout << "Clause: ~x" << m+1 << "" << p+1 << ", ~x" << m+1 << "" << q+1 << std::endl;
                }
            }
        }  
        
        // CLAUSE 4: Check edgelist clause
        //std::cout << "Clause 4: \n";
        for (int e = 0; e < edgeList.size(); e++) {
            //std::cout << "(" << edgeList[e][0]+1 << ", " << edgeList[e][1]+1 << "): ";
            for (int i = 0; i < k; i++) {
                clauseFour[e].push(x[i][edgeList[e][0]]);
                clauseFour[e].push(x[i][edgeList[e][1]]);
                //std::cout << "x" << i+1 << "" << edgeList[e][0]+1 << " x" << i+1 << "" << edgeList[e][1]+1 << " ";
            } 
            solver->addClause(clauseFour[e]);
            //std::cout << std::endl;
        }

        bool res = solver->solve();
        //std::cout << "The result is: " << res << " for k = " << k << "\n";
        //std::cout << "k = " << k << " ";
        if (res == 1) {
            for (int i = 0; i < k; i++) { 
                for (int j = 0; j < n; j++) { 
                    //std::cout << "x" << (i + 1) << (j + 1) << "=" << Minisat::toInt(solver->modelValue(x[i][j])) << " ";
                    if (Minisat::toInt(solver->modelValue(x[i][j])) == 0) {
                        //std::cout << "x" << (i + 1) << (j + 1) << "=" << Minisat::toInt(solver->modelValue(x[i][j])) << " ";
                        //cover.append(std::to_string(j+1) + " ");
                        vertexCover.push_back(j+1);
                        //count += 1;
                    }
                }
                //std::cout << std::endl;
            }
        } else {
            //cover.append("Failed at k = " + std::to_string(k));
            break;
        }
        
        std::sort(vertexCover.begin(), vertexCover.end());          // Sort vertex cover
        for (int i = 0; i < vertexCover.size(); i++) {              // Recording vertex cover
            cover.append(std::to_string(vertexCover[i]) + " ");
        }
        cover.append("(" + std::to_string(vertexCover.size()) + ")");
        minCover = cover;
        solver.reset (new Minisat::Solver());
    }
    //std::cout << cover << std::endl;
    
    return minCover;
}

/** 
 * Greedy algorithm 1 implementation
 */
std::string Matrix::greedySolver1() {
    std::string cover = "VC-GREEDY-1: ";
    std::map<int,std::vector<int>> edgesDict = {};
    std::vector<int> vertexCover = {};
    std::vector<int> removeVertex;                          // Storage for vertices to be removed from edgesDict
    int currentVertex;                                      // Storage for vertex with highest number of adjacent vertice
    int vertexSize;                                         // Storage for number of adjacent vertice (comparison)

    for (int row = 0; row < graph[0].size(); row++) {       // Create map of edges and adjList
        int index = 0;                                      // Setting index value to record
        edgesDict[row+1] = {};
        for (int col : graph[row]) {                        // Looping through column values of graph
            index += 1;
            if (col != 0) {                                 // If index value is adjacent, add to adjList
                edgesDict[row+1].push_back(index);
            }
        }
    }

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
        //std::cout << "Vertex Cover: " << currentVertex << std::endl;

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

        // for (const auto& vertexKey : edgesDict) {           // Print edgesDict
        //     std::cout << vertexKey.first << ": ";           // Print the key
        //     for (int adjList : vertexKey.second) {
        //         std::cout << adjList << " ";                // Print values in the vector
        //     }
        //     std::cout << std::endl;
        // }

    }

    std::sort(vertexCover.begin(), vertexCover.end());      // Sort vertex cover
    for (int i = 0; i < vertexCover.size(); i++) {
        cover.append(std::to_string(vertexCover[i]) + " ");
    }
    cover.append("(" + std::to_string(vertexCover.size()) + ")");
    
    return cover;
}

/**
 * Greedy algorithm 2 implementation
 */
std::string Matrix::greedySolver2() {
    std::string cover = "VC-GREEDY-2: ";
    std::map<int,std::vector<int>> edgesDict = {};
    std::vector<int> vertexCover = {};
    std::vector<std::array<int,2>> removeEdge;              // Storage for vertices to be removed from edgesDict
    std::vector<int> removeVertex;                          // Storage for vertices to be removed from edgesDict
    int x,y, px, py;                                        // Storage for vertex with highest number of adjacent vertice
    int sum, highestSum;                                    // Storage for number of adjacent vertice (comparison)

    for (int row = 0; row < graph[0].size(); row++) {       // Create map of edges and adjList
        int index = 0;                                      // Setting index value to record
        edgesDict[row+1] = {};
        for (int col : graph[row]) {                        // Looping through column values of graph
            index += 1;
            if (col != 0) {                                 // If index value is adjacent, add to adjList
                edgesDict[row+1].push_back(index);
            }
        }
    }

    while (edgeList.size() > 0) {
        px = 0;
        py = 0;
        highestSum = 0;
        removeVertex = {};
        removeEdge = {};

        for (int i = 0; i < edgeList.size(); i++) {         // Searching for edge with most adjacent vertices
            //std::cout << "(" << edgeList[i][0] << ", " << edgeList[i][1] << ")" << std::endl;
            sum = 0;
            x = edgeList[i][0]+1;
            y = edgeList[i][1]+1;
            for (const auto& vertexKey : edgesDict) {       // Sums number of adjacent vertice in edge
                if (vertexKey.first == x || vertexKey.first == y) {
                    sum += vertexKey.second.size();
                }
            }
            //std::cout << "(" << x << "," << y << ") has " << sum << " adjacent neighbors" << std::endl;
            if (sum > highestSum) {                         // If vertex pair has most adjacent vertices, record it
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

        // for (const auto& vertexKey : edgesDict) {           // Print edgesDict
        //     std::cout << vertexKey.first << ": ";           // Print the key
        //     for (int adjList : vertexKey.second) {
        //         std::cout << adjList << " ";                // Print values in the vector
        //     }
        //     std::cout << std::endl;
        // }

        for (int i = 0; i < edgeList.size(); i++) {         // Records all edges in edgeList that have vertice in vertexCover
            if (px == edgeList[i][0]+1 || px == edgeList[i][1]+1 || py == edgeList[i][0]+1 || py == edgeList[i][1]+1) {
                removeEdge.push_back({edgeList[i][0],edgeList[i][1]});
                //std::cout << "Removed: (" << edgeList[i][0]+1 << "," << edgeList[i][1]+1 << ")" << std::endl;
            }
        }
        
        for (const auto& edge : removeEdge) {               // Removes edges in edgeList that have been covered
            auto index = std::find(edgeList.begin(), edgeList.end(), edge);
            edgeList.erase(index);
            //std::cout << "Removed: " << edge[0]+1 << "," << edge[1]+1 << std::endl;
        }

        // std::cout << "Remains of edgeList: " << std::endl;
        // for (int i = 0; i < edgeList.size(); i++) {
        //     std::cout << "(" << edgeList[i][0]+1 << ", " << edgeList[i][1]+1 << ")" << std::endl;
        // }
        //std::cout << "Size of edgeList: " << edgeList.size() << std::endl;
    }

    std::sort(vertexCover.begin(), vertexCover.end());      // Sort vertex cover
    for (int i = 0; i < vertexCover.size(); i++) {
        cover.append(std::to_string(vertexCover[i]) + " ");
    }
    cover.append("(" + std::to_string(vertexCover.size()) + ")");
    
    return cover;
}

