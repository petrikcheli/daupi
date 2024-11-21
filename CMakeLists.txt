cmake_minimum_required(VERSION 3.16)

project(daupi LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Boost_USE_STATIC_LIBS OFF)

# Подключаем библиотеки Boost, PortAudio, и Opus
find_package(Boost COMPONENTS thread filesystem system program_options date_time REQUIRED)
find_package(portaudio REQUIRED)
find_package(Opus CONFIG REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Проверка Boost
if (${Boost_FOUND})
    message("Found Boost " ${Boost_VERSION})
else()
    message("Couldn't find Boost")
endif()

# Добавляем поддиректории для исходников и тестов
add_subdirectory(src)
#add_subdirectory(tests)

# Устанавливаем GNU пути
include(GNUInstallDirs)
install(TARGETS daupi
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
