cmake_minimum_required(VERSION 3.20.0)

project(calc C)

add_executable(${PROJECT_NAME} calc.c)

if("${CMAKE_C_COMPILER_ID}" MATCHES "MSVC")
    set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/LARGEADDRESSAWARE")
    target_compile_options(${PROJECT_NAME} PRIVATE /arch:AVX /utf-8)
elseif("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
    target_link_options(${PROJECT_NAME} PRIVATE -static-libstdc++ -static)
endif()
