"""
Module for reading and writing instances and solutions.
"""

from .write import write_instance
from .read import read_instance
from .random_instance import random_instance
from .solution import write_solution, read_solution, SolutionEncodingError

