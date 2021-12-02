# 2IMA15CG

Repository for participating in [the Fourth Computational Geometry Challenge, as part of CG Week in 
Berlin, Germany, June 6-10, 2022](https://cgshop.ibr.cs.tu-bs.de/competition/cg-shop-2022/), as part 
of the 2IMA15 Geometric Algorithm course.

The problem is solving the Minimum Partition into Plane Subgraphs: partitioning a graph into a minimum number of planar subgraphs. A related problem (partitioning into a minimum number of plane spanning trees) is tackled in https://arxiv.org/pdf/1707.05440.pdf.

## Source files explanation

Source files can be seperated into data structures and solvers/searchers. Solvers and searchers are located in their respective directories in src/. Data structures include basic problem Instance and Solution classes, alongside Edges and Vertices. Auxilliary data structures like a 2d bitmap cache for efficient access are also located in the main src/ folder.

The instances/ folder contains the problem instances defined by the challenge. benchmark/ contains a script to benchmark new implementations against previous ones. verify/ contains the provided tools for verifying solutions locally. include/ contains third-party libraries such as a json parser for c++.

## How to build

`mkdir build`

`cd build`

`cmake ..`

`make`


## Checklist

- [x] Add functionality to read test instances
- [x] Add functionality to write solutions
- [ ] Add functionality to visualize instances
- [x] Define relevant subproblems
- - [x] Subproblem 1: Transform to graph coloring problem
- - [x] Subproblem 2: Solve graph coloring problem
- [ ] Implement algorithms to solve the defined subproblems 
- - [x] Transformation to graph coloring instance
- - [x] Greedy algorithm for graph coloring
- - [x] Greedy (degree variant) algorithm for graph coloring
- - [x] Greedy (saturation variant) algorithm for graph coloring
- - [ ] Tabu search algorithm for graph coloring
- - [ ] Evolutionary algorithm for graph coloring
- [ ] Implement an algorithm to solve the main problem
- [ ] Generate additional test instances
