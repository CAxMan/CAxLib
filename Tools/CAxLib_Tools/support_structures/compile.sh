#!/bin/bash

CAXLIB_INCLUDE_DIR=/Users/cino/Documents/research/devel/lib/CAxLib
LIBZIP_INCLUDE_DIR=/usr/local/Cellar/libzip/1.3.0/include

g++ -I $CAXLIB_INCLUDE_DIR -I $LIBZIP_INCLUDE_DIR -Wno-c++11-extensions -ltinyxml2 -lzip -lz -o supports_service main.cpp