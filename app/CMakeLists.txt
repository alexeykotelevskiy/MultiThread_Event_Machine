add_library(EmCoreLib STATIC
   EmCore.cpp
)

target_include_directories(EmCoreLib 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)