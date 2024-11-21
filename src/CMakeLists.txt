add_executable(daupi
    WIN32
    main.cpp
    Audio/audio.h Audio/audio.cpp
    Client/client.h Client/client.cpp
    Audio/Audio_parametrs.h
    ui/mainwindow.h ui/mainwindow.cpp ui/mainwindow.ui
    ../include/main_io.h
)

# Линкуем библиотеки
target_link_libraries(daupi
    Qt${QT_VERSION_MAJOR}::Widgets
    portaudio
    Opus::opus
    Boost::thread
    Boost::filesystem
    Boost::system
    Boost::program_options
    Boost::date_time
)

# Для Windows добавляем специфичные библиотеки
if(WIN32)
  target_link_libraries(daupi wsock32 ws2_32)
endif()

# Включаем заголовочные файлы
target_include_directories(daupi PRIVATE ${Boost_INCLUDE_DIRS})

# set_target_properties(daupi PROPERTIES
#     WIN32_EXECUTABLE TRUE
# )

# if(QT_VERSION_MAJOR EQUAL 6)
#     qt_finalize_executable(daupi)
# endif()
