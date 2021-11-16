from typing import Union, Dict, Any, List, Tuple
from networkx.utils import open_file
import json


@open_file(0, mode="w")
def write_solution(path_or_file, instance: Union[str, Dict[str, Any]],
                   colors: List[int]) -> None:
    """Writes out the coloring to a path or file in the solution format.

    Args:
        path: Any path to a file, fp or str.
        instance: A string instance id or a dict as returned by read_instance. 
        colors (list): Colors for the edges of the instance.
    """
    if isinstance(instance, str):
        inst_id = instance
    else:
        inst_id = instance['id']
    output = {
        'type': 'Solution_CGSHOP2022',
        'instance': inst_id,
        'num_colors': len(set(colors)),
        'colors': colors
    }
    json.dump(output, path_or_file)


class SolutionEncodingError(Exception):
    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return self.msg


def __raise_format_error(message, meta):
    if meta is None:
        raise SolutionEncodingError(message)
    file_in_zip = meta['zip_info']['file_in_zip']
    err = SolutionEncodingError(f"Error in file '{file_in_zip}': {message}")
    err.zip_file = meta['zip_info']['zip_file']
    err.file_in_zip = file_in_zip
    raise err


def process_solution_json(data, meta=None):
    if not isinstance(data, dict) or data.get('type', None) != 'Solution_CGSHOP2022':
        __raise_format_error('Not a valid CGSHOP2022 solution file!', meta)
    num_colors = data.get('num_colors', None)
    if not isinstance(num_colors, int) or num_colors < 0:
        __raise_format_error("Number of colors must be a non-negative integer!", meta)
    instance = data.get('instance', None)
    if not isinstance(instance, str) or not instance:
        __raise_format_error('Instance ID must be a non-empty string!', meta)
    colors = data.get('colors', None)
    if not isinstance(colors, list) or any((not isinstance(i, int) for i in colors)):
        __raise_format_error("Colors must be a list of integers!", meta)
    if any(e < 0 for e in colors):
        __raise_format_error("Coloring contains negative color. Colors need to start at 0.", meta)
    return {'instance': instance, 'num_colors': num_colors, 'colors': colors}


@open_file(0, mode="r")
def read_solution(path_or_file) -> Dict[str, Union[str, int, List[int]]]:
    """
    Reads the coloring stored in the given solution file.
    Args:
        path: Any path to a file, fp or str.
    """
    data = json.load(path_or_file)
    return process_solution_json(data)

