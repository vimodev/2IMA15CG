from cgshop2022utils.io import read_instance

instance = read_instance("../instances/reecn3382.instance.json")

# INSTANCE FORMAT:

# {
#         "type": "Instance_CGSHOP2022",
#         "id": "unique name of instance",
#         "meta": dict, # with information on the instance (e.g., the polygon for visibility instances)
#         "graph": networkx.Graph, # The instance graph with typing.Tuple[int, int] as vertices.
# }

graph = instance["graph"]

# networkx example
print("Points:")
for v in graph.nodes:
    print(v)
print("Edges:")
for v,w in graph.edges:
    print(v, "<->", w)