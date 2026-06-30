cmake_minimum_required(VERSION 3.16)

project(NovaShellGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put every executable in build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Qt6 REQUIRED COMPONENTS Widgets)

find_library(READLINE_LIBRARY readline REQUIRED)

qt_standard_project_setup()

# -------------------------------
# Core library
# -------------------------------
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

# -------------------------------
# CLI executable
# -------------------------------
add_executable(NovaShell
    ../CLI/main.c
    ../CLI/shell.c
)

target_link_libraries(NovaShell
    PRIVATE
        NovaShellCore
        ${READLINE_LIBRARY}
)

# -------------------------------
# GUI executable
# -------------------------------
qt_add_executable(NovaShellGUI
    app/main.cpp
    app/MainWindow.cpp
    widgets/ShellView.cpp
    widgets/ShellEdit.cpp
    widgets/ThemeSelector.cpp
    pty/PtySession.cpp
    widgets/TitleBar.cpp
    widgets/NavigationButtons.cpp
)

target_link_libraries(NovaShellGUI
    PRIVATE
        Qt6::Widgets
)