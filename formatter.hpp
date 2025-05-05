#ifndef FORMATTER_HPP
#define FORMATTER_HPP

#include <vector>
#include <array>

std::string commandFormatter(std::string input);
int nCommand(std::string input);
std::vector<std::array<int,3>> eCommand(std::string input);
std::array<int,2> sCommand(std::string input);


#endif
