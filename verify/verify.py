from cgshop2022utils.verify import verify_coloring
from cgshop2022utils.io import read_instance, read_solution
import sys

if (len(sys.argv) != 2):
    print("expected instance argument, example:")
    print("python3 verify.py reecn3382")
    exit()

inst = sys.argv[1]

INSTANCE_DIRECTORY = "../instances/"
SOLUTION_DIRECTORY = "../build/"

instance = read_instance(INSTANCE_DIRECTORY + inst + ".instance.json")
solution = read_solution(SOLUTION_DIRECTORY + inst + ".json") # solution['instance'] should be instance['id']
error, num_colors = verify_coloring(instance, solution['colors'], expected_num_colors=solution['num_colors'])

if error is None:
    print("valid!")
else:
    print("==== Invalid! ====")
    print(error.message)
