import networkx as nx
from networkx.utils import open_file
import json


@open_file(0, mode="w")
def write_instance(path, g: nx.Graph, id: str, meta: dict):
    """writes out the networkx.Graph to path with some additional info

    Args:
        path: any path to a file, fp or str.
        g (nx.Graph): given graph. nodes are formatted as (x,y) and receive an additional "idx" keys for processing
        id (str): a unique name for this instance
        meta (dict): meta information for later use
    """
    n = g.number_of_nodes()
    m = g.number_of_edges()
    x = []
    y = []
    edge_i = []
    edge_j = []

    for idx, node in enumerate(g.nodes):
        g.nodes[node]["idx"] = idx
        x.append(node[0])
        y.append(node[1])

    node_idx = nx.get_node_attributes(g, "idx")
    for edge in g.edges:
        edge_i.append(node_idx[edge[0]])
        edge_j.append(node_idx[edge[1]])

    json.dump(
        obj={
            "type": "Instance_CGSHOP2022",  # type of this file - instance for the cgshop 2022 competition
            "id": id,  # id of this instance
            "meta": meta,  # meta-dict for later use
            "n": n,  # number of nodes
            "m": m,  # number of edges
            "x": x,  # x-coordinates of the nodes. positional!
            "y": y,  # y-coordinates of the nodes. positional!
            "edge_i": edge_i,  # positional node indices of the 1st point of an edge. positional!
            "edge_j": edge_j,  # positional node indices of the 2nd point of an edge. positional!
        },
        fp=path,
    )
