#include <iostream>
#include "formatter.hpp"
#include "matrix.hpp"

#include <array>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>

/**
 * Checks initial user input for correct command characters and returns the character as a string.
 * 
 * @param input the user input
 * @return the character command (N,E,S) as a string
 */
std::string commandFormatter(std::string input) {
    std::regex commandPattern(R"(^\s*(V|E|S|G)\s*)");                                 // Matches N, E, or S and optional whitespace
    std::regex errorPattern(R"([^NESG0-9,-{}\(\)\s]+)");                             // Matches all characters that are not in {(<int>,<int>,<int>)},
    std::smatch s;
    std::string command;
    
    std::regex_search(input, s, errorPattern);                                      // Checks for invalid characters in input
    if (s[0] != "") {
        std::cerr << "Error: Input contains invalid characters.\n";
        return "F";
    }

    std::regex_search(input, s, commandPattern);                                    // Checks for valid commands
    if (s[0] == "") {
        std::cerr << "Error: Invalid command format. Must use N, E, or S.\n";
        return "F";
    } else {
        return s[1];                                                
    }
}

/**
 * Check the specific format of N command and outputs errors for incorrect format.
 * 
 * @param input the user input
 * @return the size of the graph
 */
int nCommand(std::string input) {
    std::regex NPattern(R"(\s*V\s+(-?\d+)\s*)");                                    // Matches N <int> and optional whitespaces
    std::smatch s;

    std::regex_match(input, s, NPattern);
    if (s[0] == "") {                                                               // Checks N <int> format
        std::cerr << "Error: Invalid N command format. Input should take the form N <int>.\n";
        return -1;
    } else if ( stoi(s[1]) < 0) {                                                   // Checks <int> is positive
        std::cerr << "Error: Invalid N command format. N should have a positive integer value.\n";
        return -1;
    } else {
        return stoi(s[1]);      
    }
}

/**
 * Check the specific format of E command and outputs errors for incorrect format.
 * 
 * @param input the user input
 * @return the list of edges to be added to the graph
 */
std::vector<std::array<int,3>> eCommand(std::string input) {
    std::regex edgeListPattern(R"(\{(.*)\}$)");                                     // Matches everything between two {}
    //std::regex edgePattern(R"(\(\s*(-?\d+)\s*,\s*(-?\d+)\s*,?\s*(-?\d+)?\s*\)+)");  // Matches (<int>, <int>, <int>) including optional whitespace
    std::regex edgePattern(R"(<\s*(-?\d+)\s*,\s*(-?\d+)\s*,?\s*(-?\d+)?\s*>+)");
    // Matches E { (<int>, <int>, <int>), (<int>, <int>, <int>) } where the first term (<int>, <int>, <int>), and the third <int> are optional, accounts for whitespaces
    //std::regex EPattern(R"(\s*E\s+(\{\s*(\(\s*(-?\d+)\s*,\s*(-?\d+)\s*,?\s*(-?\d+)?\s*\)\s*,\s*)*(\s*\(\s*(-?\d+)\s*,\s*(-?\d+)\s*,?\s*(-?\d+)?\s*\))\s*\}\s*))");
    std::regex EPattern(R"(\s*E\s+(\{\s*(<\s*(-?\d+)\s*,\s*(-?\d+)\s*,?\s*(-?\d+)?\s*>\s*,\s*)*(\s*<\s*(-?\d+)\s*,\s*(-?\d+)\s*,?\s*(-?\d+)?\s*>)\s*\}\s*))");
    std::smatch s;
    int v1, v2, w = 1;                                                              // Vertex 1, Vertex 2, and weight automatically set to 1
    std::vector<std::array<int,3>> returnList;

    std::regex_match(input,s, EPattern);                                            // Checks E { (<int>, <int>, <int>)} format, enables multiple edges input
    if (s[0] == "") {
        std::cerr << "Error: Invalid E command format. Input should take the form E { (<int>, <int>, <int>) }.\n";
        return { { -1, -1, -1 } };
    }

    std::sregex_iterator edges(input.begin(), input.end(), edgePattern);            // After validation of E command format, checks (<int>, <int>, <int>)
    std::sregex_iterator edgesEnd;

    while (edges != edgesEnd) {                                                     // Extracts integers from each edge inputted
        v1 = stoi(edges->str(1));
        v2 = stoi(edges->str(2));
        if (edges->str(3) != "") {                                                  // If weight was specified, set weight to new weight
            w = stoi(edges->str(3));
        }

        returnList.push_back( {v1,v2,w} );
        ++edges;
    }

    return returnList;
}

/**
 * Check the specific format of S command and outputs errors for incorrect format.
 * 
 * @param input the user input
 * @return the source and target vertice
 */
std::array<int,2> sCommand(std::string input) {
    std::regex SPattern(R"(\s*S\s+(-?\d+)\s+(-?\d+)\s*)");                          // Matches S <int> <int> and optional whitespaces
    std::smatch s;

    std::regex_match(input, s, SPattern);
    if (s[0] == "") {                                                               // Checks S <int> <int> format
        std::cerr << "Error: Invalid S command format. Input should take the form S <int> <int>.\n";
        return { -1, -1 };
    } else if ( stoi(s[1]) < 0 || stoi(s[2]) < 0) {                                 // Checks <int> is positive
        std::cerr << "Error: Invalid S command format. S should have a positive integer values.\n";
        return { -1, -1 };
    } else if ( stoi(s[1]) == stoi(s[2]) ) {                                        // Checks for two unique integers
        std::cerr << "Error: Invalid S command format. Choose two unique integer values.\n";
        return { -1, -1 };
    } else {
        return { stoi(s[1]), stoi(s[2]) };          
    }
}

