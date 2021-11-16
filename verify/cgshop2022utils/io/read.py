import typing

import networkx as nx
from networkx.utils import open_file
import json


@open_file(0, mode="r")
def read_instance(path) -> typing.Dict[str, typing.Any]:
    """reads an json-instance of the given path

    Args:
        path: any path to a file, fp or str.

    Returns:
        dict with the following string keys:
        "type": Instance_CGSHOP2022, "id": str, "meta":dict, "graph": networkx.Graph}
    """
    data = json.load(path)

    if data["type"] != "Instance_CGSHOP2022":
        raise ValueError("Not an Instance_CGSHOP2022 file")
    if data["n"] != len(data["x"]) or data["n"] != len(data["y"]):
        raise ValueError("Instance file corrupted: Number of nodes does not match")
    if data["m"] != len(data["edge_i"]) or data["m"] != len(data["edge_j"]):
        raise ValueError("Instance file corrupted: Number of edges does not match")

    g = nx.Graph()

    node_idx = {}
    for i, pos in enumerate(zip(data["x"], data["y"])):
        g.add_node((pos[0], pos[1]), idx=i)
        node_idx[i] = (pos[0], pos[1])

    for i, points in enumerate(zip(data["edge_i"], data["edge_j"])):
        g.add_edge(node_idx[points[0]], node_idx[points[1]], idx=i)

    r = {
        "type": data["type"],
        "id": data["id"],
        "meta": data["meta"],
        "graph": g,
    }
    return r

