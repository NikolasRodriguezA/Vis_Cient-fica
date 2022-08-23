cmake_minimum_required(VERSION 2.8)

project(VisPUJ-WS9) # tells CMAKE how the executable is to be named
set(VTK_DIR "/home/sheldor/Desktop/Maestria_IA/Vis_Científica/API/VTK/VTK-bin")
find_package( VTK REQUIRED ) # tells CMAKE VTK is required to build the executable. Will fail if VTK is not found on the system
include( ${VTK_USE_FILE} ) # tells CMAKE to include the VTK headers 

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries( ${PROJECT_NAME} ${VTK_LIBRARIES} )# tells CMAKE to include the VTK libraries (dll, so. etc.)
