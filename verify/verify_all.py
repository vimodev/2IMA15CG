from cgshop2022utils.verify import verify_coloring
from cgshop2022utils.io import read_instance, read_solution
import sys
from os import listdir

INSTANCE_DIRECTORY = "../instances/"
SOLUTION_DIRECTORY = "../greedy_solutions/"

for f in listdir(SOLUTION_DIRECTORY):
    solution = read_solution(SOLUTION_DIRECTORY + f)
    instance = read_instance(INSTANCE_DIRECTORY + solution['instance'] + ".instance.json")
    error, num_colors = verify_coloring(instance, solution['colors'], expected_num_colors=solution['num_colors'])
    if error is None:
        print(f + " is valid")
    else:
        print(f + " is invalid")
        print(error.message)

# instance = read_instance(INSTANCE_DIRECTORY + inst + ".instance.json")
# solution = read_solution(SOLUTION_DIRECTORY + inst + ".json") # solution['instance'] should be instance['id']
# error, num_colors = verify_coloring(instance, solution['colors'], expected_num_colors=solution['num_colors'])
