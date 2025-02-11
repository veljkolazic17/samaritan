# CMakeList.txt : CMake project for samaritanEngine, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.8)

project(Samaritan)

include(./CommonMacros.cmake)

SETUP_APP(Samaritan_Vulkan_WindowsAPI vulkanapi Win true WINDOW_NATIVE_WIN true true tool)
