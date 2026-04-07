cmake_minimum_required(VERSION 3.16)

project(airoport VERSION 0.1 LANGUAGES CXX)

# Автоматическая обработка Qt-макросов:
#   AUTOMOC — обрабатывает Q_OBJECT (генерирует moc-файлы)
#   AUTOUIC — обрабатывает .ui файлы (генерирует ui_*.h)
#   AUTORCC — обрабатывает .qrc ресурсы

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Стандарт C++17 — нужен для Qt 6

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#  Ищем Qt любой мажорной версии (6 или 5)
#    Результат: переменная QT_VERSION_MAJOR = 5 или 6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)

#  Ищем конкретную версию по найденному номеру
#    Если QT_VERSION_MAJOR=6 → find_package(Qt6 REQUIRED ...)
#    Если QT_VERSION_MAJOR=5 → find_package(Qt5 REQUIRED ...)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Widgets
    Sql         # ← ЭТАП 2: модуль для работы с БД (QSqlDatabase, QSqlQuery, QSqlQueryModel)
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

# ③ Создаём исполняемый файл
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    # Qt 6: обёртка, на десктопе = add_executable()
    #        на Android/iOS добавляет генерацию пакетов
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION   # "я сам вызову финализацию позже"
        ${PROJECT_SOURCES}
        database.h database.cpp
    )
else()
    # Qt 5: qt_add_executable() не существует
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

# ④ Линковка — автоподстановка версии
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets  # → Qt6::Widgets или Qt5::Widgets
    Qt${QT_VERSION_MAJOR}::Sql      # ← ЭТАП 2
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.airoport)
endif()
set_target_properties(airoport PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS airoport
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ⑤ Финализация — генерация манифестов для мобильных платформ
#    На десктопе ничего не делает, но Qt Creator ожидает этот вызов
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
