cmake_minimum_required(VERSION 3.15)

project(sqlite)

add_library(sqlite STATIC sqlite3.c shell.c)
