#  NovaShell

A modular Unix-like shell implemented in C to explore Linux process management, inter-process communication, signal handling, and shell architecture.

![NovaShell Demo](images/Screenshot%20From%202026-06-16%2015-54-01.png)

## Features

- Interactive shell
- Colored prompt with current working directory
- Built-in commands ('cd', 'exit')
- GNU Readline integration (history & line editing)
- External command execution
- Input and output redirection ('<', '>', '>>')
- Arbitrary-length pipelines
- Environment variable expansion (`$HOME`, `$PATH`, ...)
- Signal handling (Ctrl+C)





##  ARCHITECTURE

```
                            NovaShell Architecture

                                  User Input
                                     │
                                     ▼
                      ┌──────────────────────────┐
                      │   GNU Readline Library   │
                      │  (Input & History Mgmt)  │
                      └────────────┬─────────────┘
                                   │
                                   ▼
                      ┌──────────────────────────┐
                      │      shell.c             │
                      │   Main Shell Loop        │
                      │  Command Dispatcher      │
                      └────────┬───────┬─────────┘
                               │       │
                ┌──────────────┬┴───────┴───────────────┐
                │              │                        │
                ▼              ▼                        ▼
       ┌─────────────────┐ ┌─────────────────┐ ┌──────────────────┐
       │    parser.c     │ │  builtins.c     │ │ environment.c    │
       │                 │ │                 │ │                  │
       │ • Tokenization  │ │ • cd, pwd, exit │ │ • $VAR expansion │
       │ • Arg parsing   │ │ • env variables │ │ • Path handling  │
       └────────┬────────┘ └────────┬────────┘ └────────┬─────────┘
                │                   │                   │
                └───────────────────┼───────────────────┘
                                    ▼
                      ┌──────────────────────────┐
                      │    executor.c            │
                      │  Command Execution Logic │
                      │  Route to builtin/extern │
                      └────────────┬─────────────┘
                                   │
                ┌──────────────────┴──────────────────┐
                │                                     │
                ▼                                     ▼
       ┌─────────────────────────┐      ┌──────────────────────────┐
       │    process.c            │      │    launcher.c            │
       │                         │      │                          │
       │ • fork() process        │      │ • execvp() execution     │
       │ • Pipe creation         │      │ • I/O redirection (>/<)  │
       │ • waitpid() handling    │      │ • dup2() file descriptor │
       │ • Signal management     │      │ • Output append (>>)     │
       └────────────┬────────────┘      └────────────┬─────────────┘
                    │                                │
                    └────────────┬─────────────────┘
                                 ▼
                    Linux Kernel System Calls
        (fork, execvp, waitpid, dup2, pipe, signal, wait)
```









## Project Structure

```
NovaShell/
├── Makefile                 # Build configuration
├── README.md                # Project documentation
├── include/                 # Header files (public interfaces)
│   ├── parser.h            # Command tokenization & parsing
│   ├── executor.h          # Command execution routing
│   ├── launcher.h          # External program launching
│   ├── process.h           # Process management (fork, wait)
│   ├── shell.h             # Main shell loop interface
│   ├── builtins.h          # Built-in commands (cd, exit)
│   ├── colours.h           # Terminal color codes
│   └── environment.h       # Environment variable handling
├── src/                     # Implementation files
│   ├── main.c              # Program entry point
│   ├── shell.c             # Main shell loop & command dispatch
│   ├── parser.c            # Tokenization & argument parsing
│   ├── executor.c          # Command routing & execution control
│   ├── launcher.c          # execvp() wrapper & I/O redirection
│   ├── process.c           # fork(), waitpid(), pipe management
│   ├── builtins.c          # Built-in command implementations
│   └── environment.c       # Environment variable expansion
└── NovaShell                    # Compiled shell executable
```

## Dependencies

- **C Compiler**: GCC or Clang (C99 standard or later)
- **GNU Readline Library**: For input handling and history
  - Ubuntu/Debian: `sudo apt-get install libreadline-dev`
  - macOS: `brew install readline`
- **POSIX-compliant system**: Linux, macOS, or Unix
- **Make**: Build automation tool

## Installation

### Build from Source

```bash
git clone https://github.com/KoushikKadiyala/NovaShell.git
cd NovaShell
make
```

This will compile the shell and generate the `NovaShell` executable in the project root.

### Running the Shell

```bash
./NovaShell
```

Once started, you'll see an interactive prompt where you can enter shell commands.


##  Examples

### Basic Commands

```bash
$ pwd
/home/user/my_shell

$ ls -l
total 48
drwxr-xr-x  3 user user  4096 Jun 14 10:30 include/
drwxr-xr-x  3 user user  4096 Jun 14 10:30 src/
-rw-r--r--  1 user user  1024 Jun 14 10:30 Makefile
-rwxr-xr-x  1 user user 24576 Jun 14 10:30 NovaShell

$ echo hello
hello
```

### I/O Redirection

```bash
$ echo hello > file.txt
$ cat < file.txt
hello

$ cat file.txt
hello
```

### Piping

```bash
$ cat file.txt | grep hello
hello

$ ls | grep cpp | sort | wc
      3       3      42

$ ls | grep ".c" | wc -l
8
```

### Environment Variables

```bash
$ echo $HOME
/home/user

$ echo $PATH
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin
```

## What I learned

This Project helped me understand:

- Linux process management
- fork(), execvp(), waitpid()
- Inter-process communication using pipes
- File descriptor manipulation using dup2()
- Signal handling
- Shell parsing
- Modular software architecture in C

## Key Design Decisions

- Modular architecture
- Arbitrary-length pipeline implementation
- Separation of parsing and execution
- Dedicated launcher for child process setup

## Challenges

- Implementing arbitrary-length pipelines
- Correct file descriptor management
- Implementing signal handling while keeping the shell process alive.
- Parsing shell syntax

## Future Work

- Quote handling
- Background jobs
- Command history persistence
- Tab completion
- Native desktop GUI built on top of the shell engine

## Author

**Koushik Naidu Kadiyala**
B.Tech, Electronics & Communication Engineering
Indian Institute of Technology Guwahati
GitHub: https://github.com/KoushikKadiyala

## License

MIT License

Copyright (c) 2026 Koushik Kadiyala

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

