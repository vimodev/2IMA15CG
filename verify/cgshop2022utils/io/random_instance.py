# straight copy from https://gitlab.ibr.cs.tu-bs.de/krupke/minimum-plane-partition-playground/-/blob/master/mppalgs/random_instance.py 

from random import random
import networkx as nx

def random_instance(n=15, p=0.5) -> nx.Graph:
    points = [(int(round(10_000 * random())), int(round(10_000 * random()))) for i in range(n)]

    G = nx.binomial_graph(len(points), p)

    instance = nx.Graph()
    instance.add_nodes_from(points)
    for e in G.edges:
        instance.add_edge(points[e[0]], points[e[1]])

    return instance
