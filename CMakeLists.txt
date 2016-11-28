#Computer graphics homework 

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#SET(CMAKE_C_COMPILER g++)

PROJECT(Compgraph CXX)

SET(PACKAGE_NAME "Compgraph")

SUBDIRS(B-Spline_Curve NURBS_Curve Subdivision_Curve )#NURBS_Surface

SET(CMAKE_VERBOSE_MAKEFILE on)

