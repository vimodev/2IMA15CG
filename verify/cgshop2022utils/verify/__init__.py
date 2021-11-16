try:
    from coloring_verifier import Point, Segment, ColoringError,\
                                   do_intersect as _do_intersect, verify_coloring as _verify_coloring
except ModuleNotFoundError:
    import warnings
    warnings.warn("Could not import C++ extension for verification of colorings; verifying colorings will fail."\
                  "To verify colorings, please perform a proper installation of the cgshop2022utils package.")
    pass


import typing as _t
import networkx as _nx
import math as _math


def _to_segments(graph):
    edge_to_index = _nx.get_edge_attributes(graph, "idx")
    if len(edge_to_index) == 0:
        edge_to_index = {e: i for i, e in enumerate(graph.edges)}
    index_to_segment = {edge_to_index[e]: (int(e[0][0]), int(e[0][1]), int(e[1][0]), int(e[1][1])) for e in graph.edges}
    return [index_to_segment[i] for i in range(len(index_to_segment))]

def verify_coloring(instance, coloring: _t.List[int], expected_num_colors: _t.Optional[int]=None):
    """
    Verify a coloring.

    Args:
        instance: An instance (can be a dict as returned by io.read_instance, a networkx graph of segments, or a list of segments)
        coloring: A coloring as list of integers.
        expected_num_colors: The number of colors expected for this instance.
    """
    if isinstance(instance, dict):
        instance = instance['graph']
    if isinstance(instance, _nx.Graph):
        instance = _to_segments(instance)
    err, used_num_colors = _verify_coloring(instance, coloring)
    if err is not None:
        return err, used_num_colors
    if expected_num_colors is not None and used_num_colors > expected_num_colors:
        msg = f"Given number of colors ({expected_num_colors}) is less than actual number of colors ({used_num_colors})"
        return ColoringError(0, 0, used_num_colors-1, _math.nan, _math.nan, msg, False), used_num_colors
    return err, used_num_colors

def do_intersect(s1, s2):
    s1 = s1.ordered()
    s2 = s2.ordered()
    return _do_intersect(s1, s2)

