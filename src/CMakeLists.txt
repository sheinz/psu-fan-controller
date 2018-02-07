add_avr_executable(
   psu-fan-controller
   main.c
   timer.c
)

find_library(C_LIB c)
message(STATUS "avr-libc: ${C_LIB}")

avr_target_link_libraries(psu-fan-controller ${C_LIB})

