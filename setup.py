from setuptools import setup, Extension
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "kmeans",
        ["app/kmeans.cpp"],
        extra_compile_args=["-std=c++11"],
    ),
]

setup(
    name="kmeans",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
)
