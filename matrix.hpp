#ifndef MATRIX_HPP
#define MATRIX_HPP


#include <string>
#include <vector>
#include <array>

class Matrix {
    int row;
    int col;
    std::vector<std::vector<int>> graph;
    std::vector<int> distance;
    std::vector<int> parent;

    std::vector<std::array<int,3>> data;
    std::vector<std::array<int,2>> edgeList;

public:
    // Constructor and Destructor
    Matrix(int newRow, int newCol);
    ~Matrix();

    // Graph Methods
    int get(int r, int c);                                           
    void set(int r, int c, int w);                  
    void resize(int r, int c);                      
    void print();                                  
    void dijkstra(int source);
    std::string printPathway(int source, int target, int size);
    std::string greedySolver1();
    std::string greedySolver2();
    std::string vcExact(int size);
};


#endif
