project(FSCKingTower)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(FSCKingTower_SRCS FSCKingTower.cpp main.cpp widgets/plotter.cpp)
qt4_automoc(${FSCKingTower_SRCS})
add_executable(FSCKingTower ${FSCKingTower_SRCS})
target_link_libraries(FSCKingTower ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
install(TARGETS FSCKingTower RUNTIME DESTINATION bin)
