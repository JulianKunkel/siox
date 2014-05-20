project(SIOX)

set(CMAKE_C_FLAGS "-ggdb -Wall -std=gnu99 -fPIC")
set(SKELETON_TEMPLATE ${CMAKE_SOURCE_DIR}/tools/siox-skeleton-builder/template.py)

#install(DIRECTORY styles/ DESTINATION bin/styles/)

add_subdirectory(layers)

