cmake_minimum_required(VERSION 3.16)

project(ModelSim LANGUAGES CXX Fortran)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON) # Ensure moc is enabled
set(CMAKE_AUTORCC ON) # Ensure resource files are processed (if any)
set(CMAKE_AUTOUIC ON) # Ensure UI files are processed (if any)

# Find Qt6 components
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# Add your source files
set(SOURCES
    main.cpp
    widget.cpp
    widget.h
)

# Tell Qt to process the header for moc (Meta-Object Compiler)
qt6_wrap_ui(UI_FORMS_H) # Not strictly needed here as we don't have .ui files
qt6_wrap_cpp(MOC_HEADERS widget.h) # Processes the Q_OBJECT macro

# Add Fortran library
# add_library(FortranModels SHARED IMPORTED)
# set_target_properties(FortranModels PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libfortranmodels.so)

# Add your executable
add_executable(ModelSim ${SOURCES} ${MOC_HEADERS} ${UI_FORMS_H})

# Link necessary Qt libraries
target_link_libraries(ModelSim PRIVATE Qt6::Widgets)

# --- FORTRAN INTEROPERABILITY (Placeholder) ---
# In a real scenario, you would compile your Fortran code into a static or shared library
# and link it here.

# Example if you had a Fortran static library named 'libfortranmodels.a'
# add_library(fortranmodels STATIC fortran_src/my_fortran_module.f90) # You'd define this
# target_link_libraries(ModelSim PRIVATE fortranmodels) # Link it

# Or, if Fortran produces an object file directly:
# add_custom_command(
#     OUTPUT ${CMAKE_BINARY_DIR}/fortran_src/my_fortran_module.o
#     COMMAND gfortran -c ${CMAKE_CURRENT_SOURCE_DIR}/fortran_src/my_fortran_module.f90 -o ${CMAKE_BINARY_DIR}/fortran_src/my_fortran_module.o
#     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/fortran_src/my_fortran_module.f90
# )
# target_link_libraries(ModelSim PRIVATE ${CMAKE_BINARY_DIR}/fortran_src/my_fortran_module.o)