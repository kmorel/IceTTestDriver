project(IceTTestDriver)

cmake_minimum_required(VERSION 2.8)

find_package(IceT REQUIRED)
find_package(MPI REQUIRED)

mark_as_advanced(clear
  MPI_INCLUDE_PATH
  MPI_LIBRARY
  MPI_EXTRA_LIBRARY
  )

find_path(IceT_SRC
  NAMES tests/SimpleTiming.c
  DOC "IceT source directory"
  )

if (NOT IceT_SRC)
  message(SEND_ERROR "Could not find IceT source directory.  Set IceT_SRC.")
endif (NOT IceT_SRC)

include_directories(
  ${ICET_INCLUDE_DIRS}
  ${MPI_INCLUDE_PATH}
  ${IceT_DIR}/tests
  )

set(SRC
  IceTTestDriver.cpp
  ${IceT_SRC}/tests/SimpleTiming.c
  ${IceT_SRC}/tests/ppm.c
  )

add_executable(IceTTestDriver ${SRC})

target_link_libraries(IceTTestDriver
  ${ICET_CORE_LIBS}
  ${ICET_MPI_LIBS}
  ${MPI_LIBRARY}
  ${MPI_EXTRA_LIBRARY}
  )
