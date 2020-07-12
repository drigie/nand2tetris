include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(hasm main.cpp Parser.cpp)
add_executable(ParserTest Parser_test.cpp)
