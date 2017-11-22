include_directories(../)

add_executable(PdReaderTest PdReaderTest.cc)
target_include_directories(PdReaderTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(PdReaderTest LINK_PUBLIC protodune-dam)

add_executable(PdWibFrameTest PdWibFrameTest.cc)
target_link_libraries(PdWibFrameTest LINK_PUBLIC protodune-dam)


INSTALL(
    TARGETS PdReaderTest PdWibFrameTest
    RUNTIME DESTINATION bin
    )
