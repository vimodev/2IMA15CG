from cgshop2022utils.io import read_instance
import glob

TARGET_DIRECTORY = "../cpp_instances"

all_instance_files = glob.glob("../instances/*.instance.json")

print("Handling " + str(len(all_instance_files)) + " instances")

# Go over all files
for file in all_instance_files:
    # Fetch data
    instance = read_instance(file)
    instance_type = instance["type"]
    instance_id = instance["id"]
    graph = instance["graph"]
    nodes = list(graph.nodes)
    edges = list(graph.edges)
    # Form output file
    output_name = TARGET_DIRECTORY + "/" + file.split("/")[-1].replace('json', 'txt')
    print(file + " -> " + output_name)
    f = open(output_name, "w+")
    f.write(instance_type + ";\n")
    f.write(instance_id + ";\n")
    f.write(";\n")
    f.write(str(len(nodes)) + ";\n")
    f.write(str(len(edges)) + ";\n")
    f.write(";\n")
    for x,y in nodes:
        f.write(str(x) + " " + str(y) + ";\n")
    f.write(";\n")
    for e in edges:
        u, v = e
        f.write(str(nodes.index(u)) + " " + str(nodes.index(v)) + ";\n")
    f.close()

# INSTANCE FORMAT:

# {
#         "type": "Instance_CGSHOP2022",
#         "id": "unique name of instance",
#         "meta": dict, # with information on the instance (e.g., the polygon for visibility instances)
#         "graph": networkx.Graph, # The instance graph with typing.Tuple[int, int] as vertices.
# }
