# 🐚 MiniShell – A Custom Unix-like Shell in C

Overview
**MiniShell** is a lightweight, Unix-like shell developed in C as part of a Linux Internals project. It replicates core functionalities of traditional shells including command execution, signal handling, environment variable support, and piping – providing a deeper understanding of system-level programming.

Features
- Custom Shell Prompt
  - Default prompt: `minishell$`
  - Customizable via the `PS1` environment variable (e.g., `PS1=user@minishell`)

- Command Execution
  - Supports external commands using `fork()` and `execvp()`
  - Built-in commands: `cd`, `pwd`, `exit`

- Special Shell Variables
  - `$$` → PID of the shell
  - `$?` → Exit status of the last executed command
  - `$SHELL` → Path of the minishell executable

- Signal Handling
  - `Ctrl+C` → Sends SIGINT to terminate foreground process
  - `Ctrl+Z` → Sends SIGTSTP to suspend foreground process and displays its PID

- Inter-Process Communication
  - Supports N-level piping (e.g., `ls | grep .c | wc -l`)

Technologies Used
- Language: C
- Concepts: System Calls, Process Control, Signals, Pipes, Environment Variables
- Tools: GCC, GDB, Makefile, Vim

---

How It Works

1. Displays a user-configurable prompt.
2. Reads and parses user input.
3. Identifies and executes:
   - Built-in commands internally.
   - External commands via child processes.
4. Handles foreground process control using signals.
5. Supports chaining commands using pipes.

Learning Outcomes
- Practical knowledge of Linux internals and system-level programming.
- Experience with parsing, process creation, and IPC.
- Confidence in debugging and building interactive terminal-based applications in C.

Sample Run

```bash
minishell$ pwd
/home/dhanya

minishell$ PS1=dhanya_shell$
dhanya_shell$ echo $$
4592

dhanya_shell$ ls | grep .c | wc -l
3
