cmake_minimum_required(VERSION 3.16)

project(NovaShellGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

add_library(NovaShellCore STATIC
    ../engine/core.c
    ../engine/parser.c
    ../engine/executor.c
    ../engine/builtins.c
    ../engine/environment.c
    ../engine/launcher.c
    ../engine/process.c
    ../engine/io.c
    ../engine/memory.c
)
target_include_directories(NovaShellCore
    PUBLIC
    ../include
)
qt_add_executable(NovaShellGUI
    app/main.cpp
    app/MainWindow.cpp
    widgets/ShellView.cpp
    widgets/ThemeSelector.cpp
    widgets/ShellEdit.cpp
    pty/PtySession.cpp
)

target_link_libraries(NovaShellGUI 
                        PRIVATE
                            Qt6::Widgets
                            NovaShellCore
 )