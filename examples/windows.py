import logging
import os
import sys
from ctypes import WinDLL
from pathlib import Path

logger = logging.getLogger(__name__)
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
    filename="import.log",
    encoding="utf-8",
    filemode="w",
)


def add_dll_directories(path_list: list[str], silent: bool = False) -> None:
    """Add directories to DLL search path on Windows (Python 3.8+)"""
    for path in path_list:
        if os.path.isdir(path):
            try:
                os.add_dll_directory(path)
                if not silent:
                    logger.info(f"Added DLL directory - Path: {path}")
            except Exception as e:
                if not silent:
                    logger.error(f"Failed to add DLL directory - Path: {path} - Error: {e}")


def import_module(pyd_name: str, pyd_dir: str = "../build", silent: bool = False) -> None:
    """
    Attempt to load a compiled C++ extension module (.pyd) from a given directory
    Also imports the module if successful
    """
    module_path = Path(pyd_dir) / pyd_name
    module_name = Path(pyd_name).stem.split(".")[0]

    # Ensure DLLs in PATH are searchable
    path_directories = os.environ.get("PATH", "").split(";")
    add_dll_directories(path_directories, silent)

    # Try to load .pyd manually
    try:
        WinDLL(str(module_path))
        if not silent:
            logger.info(f"Loaded DLL file - Path: {module_path}")
    except Exception as e:
        if not silent:
            logger.error(f"Failed to load DLL - Path: {module_path} - Error: {e}")
        return

    # Try to import the Python module
    try:
        if pyd_dir not in sys.path:
            sys.path.insert(0, pyd_dir)
        __import__(module_name)
        if not silent:
            logger.info(f"Imported module - Module: {module_name}")
    except ImportError as e:
        if not silent:
            logger.error(f"Failed to import module - Module: {module_name} - Error: {e}")


if __name__ == "__main__":
    pass
