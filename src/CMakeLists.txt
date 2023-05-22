
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third_party/include)

aux_source_directory(. src_path)
add_executable(Server ${src_path})

target_link_libraries(Server ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/lib/lualib.a)
