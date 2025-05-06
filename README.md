# The SSIR Module

I started this side project to explore the potential of using C++17 code as the foundation for a Python module. The idea is to develop a simulation program that requires high performance.

## Overview

The model simulates a grid where individuals move between different states: Susceptible, Incubated (exposed), Infected, Recovered, and Dead. You can use matplotlib in Python to visualize the statistics over time. See [`examples/visualize.py`](examples/visualize.py).

I've learned how to combine C++17 code with Python using [pybind11](https://github.com/pybind/pybind11). This setup provides the efficiency of C++ and the flexibility of Python. I also gained experience organizing a project with bindings, stub files, and writing basic CMake configurations to build and expose C++ modules to Python.

This model is mainly inspired by [Outbreak](https://meltingasphalt.com/interactive/outbreak/).

## How to Run

After building the project with CMake, you can try it out using the Python scripts in the `examples/` folder. The module is compiled into a `.pyd` file (on Windows), which you can import in Python using the helper file [`examples/windows.py`](examples/windows.py).

## Notes

This project is still in progress. I'm mainly using it as a learning tool to understand how C++ and Python can work together, how bindings are written, and how to structure a small cross-language codebase.

Have a nice day!
