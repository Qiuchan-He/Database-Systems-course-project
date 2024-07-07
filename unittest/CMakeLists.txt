set(
    UNITTEST_SOURCES
    main.cpp
    data_layouts_test.cpp
    BTreeTest.cpp
    MyPlanEnumeratorTest.cpp
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    include_directories(
        ${PROJECT_SOURCE_DIR}/third-party/catch2/include
        .
    )

    add_executable(unittest ${UNITTEST_SOURCES})
    target_link_libraries(unittest PRIVATE $<TARGET_OBJECTS:dbsys22> mutable)
endif()
