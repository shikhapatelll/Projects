cmake_minimum_required(VERSION 3.16)
project(StudyFlow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(studyflow
    src/AuthService.cpp
    src/CommandParser.cpp
    src/Date.cpp
    src/JsonRepository.cpp
    src/PlannerService.cpp
    src/ReminderService.cpp
    src/Scheduler.cpp
    src/SimpleJson.cpp
    src/StudyItem.cpp
    src/StudySession.cpp
    src/UserAccount.cpp
    src/UserData.cpp
)

target_include_directories(studyflow PUBLIC include)

add_executable(studyflow_app src/main.cpp)
target_link_libraries(studyflow_app PRIVATE studyflow)
