import re

def read_txt(file_name: str):
    """Reads file.txt output from graphGen and returns graphs as strings in list form."""
    results = []

    with open(file_name, encoding='utf-8') as file:
        for line in file:
            line = line.strip()
            if line.startswith("V"):
                results.append((line, next(file)))

    return results

def process_graphs(graphs: dict):
    """Processes graphs in string form and returns V as an int and a list of edges."""
    results = []

    for (vertex, edgeList) in graphs:
        edges = []
        matchVertex = re.match(r"V (\d+)", vertex)
        matchEdges = re.finditer(r"<(\d+),(\d+)>", edgeList)

        v = int(matchVertex.group(1))
        for match in matchEdges:
            edges.append( (int(match.group(1)),int(match.group(2))) )
        
        results.append( (v,edges) )

    return results

def cnf_maker(input: tuple, cover_size: int):
    literals = []           # Storage for all literals
    n = input[0]            # Number of vertex  
    edgeList = input[1]     # List of edges
    k = cover_size          # Size of vertex cover
    cnf = [f"k {k}",f"p cnf {n} "]    # Storage for all clauses

    # Create literals (k rows of literals)
    for i in range(0,k*n,n):
        literals.append( list(range(1,n*k+1))[i:i+n] )

    # Clause 1
    for i in range(k):
        clause = ""
        for literal in range(n):
            clause += str(literals[i][literal]) + " "
        clause += "0"
        cnf.append(clause)

    # Clause 2
    for m in range(n):              # FOR m in [1,n]
        for q in range(k):          # FOR q in [1,k]
            for p in range(q):      # FOR p in [1,k] and p < q
                cnf.append(f"-{literals[p][m]} -{literals[q][m]} 0")

    # Clause 3
    for m in range(k):              # FOR m in [1,k]
        for q in range(n):          # FOR p in [1,n]
            for p in range(q):      # for q in [1,n] and p < q      note: these are swapped in the C++ version (probably cause of handwritten example)
                cnf.append(f"-{literals[m][q]} -{literals[m][p]} 0")

    # Clause 4
    for vertex in range(len(edgeList)):     # Loops through x and y in <x,y>, print with edgeList[vertex][0] and edgeList[vertex][1]
        clause = ""
        for i in range(k):                  # Loops through size of vertex cover
            clause += str(literals[i][edgeList[vertex][0]-1]) + " "     # x must be in the cover
            clause += str(literals[i][edgeList[vertex][1]-1]) + " "     # y must be in the cover
        clause += "0"
        cnf.append(clause)
        
    cnf[1] += str(len(cnf)-2)       # Subtract 2 because the first two lines are not a clause
    return cnf

def write_txt(input: list, file_name: str):
    with open(file_name, "w") as file:
        file.write("\n".join(input))            

def main():
    results = read_txt("build/test_file.txt")
    num = process_graphs(results)
    k = 2
    output = cnf_maker(num[0], k) 
    
    write_txt(output, "build/input.cnf")


if __name__ == "__main__":
    main()

    """
    [1] [2] [3] [4] [5]
    [6] [7] [8] [9] [10]
    [11][12][13][14][15]

    x11 x21 x31 x41 x51
    x12 x22 x32 x42 x52
    x13 x23 x33 x43 x53
    """



    

