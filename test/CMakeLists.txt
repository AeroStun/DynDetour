add_executable (dyndetour_test main.cxx)
target_link_libraries (dyndetour_test PRIVATE dyndetour)
add_test (NAME dyndetour_test COMMAND dyndetour_test)
