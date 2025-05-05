# Vertex Cover

Create a polynomial reduction of the decision version of VERTEX COVER to CNF-SAT. We have
discussed the reduction in class. It is also included at the end of this assignment handout. You
are allowed to use your own reduction provided it is sound and polynomial-time. Implement the
reduction and use minisat as a library to solve the minimum VERTEX COVER problem for the
graphs that are input to your program (as in Assignment 2).
As soon as you get an input graph via the ’V’ and ’E’ specification you should compute a
minimum-sized Vertex Cover, and immediately output it. The output should just be a sequence of
vertices in increasing order separated by one space each. You can use qsort(3) or std::sort for
sorting.
Assuming that your executable is called ece650-a4, the following is a sample run of your pro-
gram:
$ ./ece650-a4
V 5
4 5
E {<1,5>,<5,2>,<1,4>,<4,5>,<4,3>,<2,4>}
The lines starting with V and E are the inputs to your program, and the last line is the output.
Note that the minimum-sized vertex cover is not necessarily unique. You need to output just one
of them.

# Run Program

See build file for executable ece650-a4-kissat. Run:
$ ./ece650-a4-kissat

Input graph into command line to see output of vertex cover.
