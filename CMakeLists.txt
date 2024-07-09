# CMakeList.txt : CMake project for samaritanEngine, include source and define
# project specific logic here.
cmake_minimum_required (VERSION 3.8)

project(Chapter1)

include(./CommonMacros.cmake)

SETUP_APP(Vulkan_WinAPI vulkanapi Win true WINDOW_NATIVE_WIN true)