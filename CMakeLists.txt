cmake_minimum_required(VERSION 2.8.7)
project(mutant)

include_directories(${PROJECT_SOURCE_DIR})

set(APP_SRC
  main.cpp
  )

add_executable(mutant ${APP_SRC})
