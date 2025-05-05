# Vertex Cover

The following is a polynomial reduction of the decision version of VERTEX COVER to CNF-SAT. 
The cover uses minisat and kissat library to solve the minimum VERTEX COVER problem for graphs
that are input into the program. Additionally, two greedy solver methods were also implemented.

Graph specifications are done via 'V' and 'E' commands and the minimum vertex cover is 
the output. Below is sample input.

```
$ ./ece650-a4-kissat
V 5
E {<1,4>,<2,3>,<2,1>,<2,5>,<4,2>,<1,3>,<5,1>}
```

The lines starting with V and E are the inputs to the program.

# Run Program

See build file for executable ece650-a4-kissat. Run:

```
$ ./ece650-a4-kissat
```

Input graph into command line to see output of vertex cover.
