cmake_minimum_required(VERSION 3.20)
project(TempMonitorOsipov)

set(CMAKE_CXX_STANDARD 14)

add_executable(TempMonitorOsipov main.cpp DS9490R.cpp DS9490R.h SmsModem.h SmsModem.cpp)
target_link_libraries(TempMonitorOsipov owcapi)