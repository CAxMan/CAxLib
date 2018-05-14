#!/bin/sh

g++ -std=c++11 -DCINOLIB_USES_TETGEN -DTETLIBRARY -DCINOLIB_USES_VTK -I./include -I./include/vtk-7.1 -L./lib -ltet -lvtkCommonCore-7.1.1 -lvtkCommonDataModel-7.1.1 -lvtkIOCore-7.1.1 -lvtkIOLegacy-7.1.1 -lvtkIOXML-7.1.1 -o tetrahedralize_bin main.cpp