# molec - Molecular Dynamics Framework
#
#  Copyright (C) 2016  Carlo Del Don  (deldonc@student.ethz.ch)
#                      Michel Breyer  (mbreyer@student.ethz.ch)
#                      Florian Frei   (frofrei@student.ethz.ch)
#                      Fabian Thuring (thfabian@student.ethz.ch)
# 
#  This file is distributed under the MIT Open Source License. 
#  See LICENSE.txt for details.

cmake_minimum_required(VERSION 2.8)

set(UNITTEST_SOURCE
    UnittestMain.c
    UnittestTimer.c
)

set(UNITTEST_HEADER
    Unittest.h
)

add_executable(molecTest ${UNITTEST_SOURCE} ${UNITTEST_HEADER} ${MOLEC_HEADERS})
target_link_libraries(molecTest molecCore)

# Link against math library (-lm)
if(NOT(${CMAKE_C_COMPILER_ID} STREQUAL "MSVC"))
    target_link_libraries(molecTest m)
endif(NOT(${CMAKE_C_COMPILER_ID} STREQUAL "MSVC"))