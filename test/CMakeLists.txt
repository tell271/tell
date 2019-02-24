add_executable(tutil tutil.cc)
add_executable(targrt targrt.cc)
add_executable(targct targct.cc)
add_executable(tmeta tmeta.cc)
add_executable(trand trand.cc)

target_link_libraries(tutil gtest)
target_link_libraries(tutil pthread)
target_link_libraries(tutil tell)

target_link_libraries(targrt gtest)
target_link_libraries(targrt pthread)
target_link_libraries(targrt tell)

target_link_libraries(targct gtest)
target_link_libraries(targct pthread)
target_link_libraries(targct tell)

target_link_libraries(trand gtest)
target_link_libraries(trand pthread)
target_link_libraries(trand tell)

add_test(tutil tutil)
add_test(targrt targrt)
add_test(targct targct)
add_test(tmeta tmeta)
add_test(trand trand)

# example: ctest -T memcheck
include (CTest)
