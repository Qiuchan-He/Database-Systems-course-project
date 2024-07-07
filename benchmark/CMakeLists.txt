add_executable(milestone1_bench milestone1.cpp)
target_link_libraries(milestone1_bench PRIVATE $<TARGET_OBJECTS:dbsys22> mutable)

add_executable(milestone2_bench milestone2.cpp)
target_link_libraries(milestone2_bench PRIVATE $<TARGET_OBJECTS:dbsys22> mutable)

add_executable(milestone3_bench milestone3.cpp)
target_link_libraries(milestone3_bench PRIVATE $<TARGET_OBJECTS:dbsys22> mutable)
