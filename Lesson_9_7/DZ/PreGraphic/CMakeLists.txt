cmake_minimum_required(VERSION 3.5)

project(PreGraphic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Widgets - GUI
# Concurrent - многопоточность (QtConcurrent::run, .then())
# PrintSupport - ОБЯЗАТЕЛЕН для QCustomPlot
find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent PrintSupport)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        graphwindow.cpp
        graphwindow.h
)


add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

# 1. Обязательный define - говорит что используем .dll, а не исходники
add_compile_definitions(QCUSTOMPLOT_USE_LIBRARY)

# 2. Путь к папке с qcustomplot2.dll (положи .dll рядом с проектом)
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 3. Путь к qcustomplot.h (если лежит в подпапке include/)
# target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 4. Линковка
target_link_libraries(${PROJECT_NAME}
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::PrintSupport       # Обязателен для QCustomPlot!
    qcustomplot2            # Имя библиотеки без .dll
)



