add_library(
    dbsys22
    OBJECT
    data_layouts.cpp
    MyPlanEnumerator.cpp
)
add_dependencies(dbsys22 Mutable)

add_executable(milestone1 milestone1.cpp)
target_link_libraries(milestone1 PRIVATE $<TARGET_OBJECTS:dbsys22> mutable)

add_executable(milestone2 milestone2.cpp)
target_link_libraries(milestone2 PRIVATE $<TARGET_OBJECTS:dbsys22> mutable)

add_executable(milestone3 milestone3.cpp)
target_link_libraries(milestone3 PRIVATE $<TARGET_OBJECTS:dbsys22> mutable)
