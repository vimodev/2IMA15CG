#!/bin/bash
c++ -O3 -Wall -shared -std=c++17 -fPIC $(python3 -m pybind11 --includes) cgshop2022utils/verify/coloring_verifier.cpp cgshop2022utils/verify/geom.hpp cgshop2022utils/verify/verify_coloring.hpp cgshop2022utils/verify/bentley_ottmann_any_intersection.hpp  -o coloring_verifier$(python3-config --extension-suffix)