<div align="center">

# Proc

[![License](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)](LICENSE)
[![Ring](https://img.shields.io/badge/Ring-1.27+-blue.svg?style=for-the-badge&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCI+PGNpcmNsZSBjeD0iMTIiIGN5PSIxMiIgcj0iMTAiIGZpbGw9Im5vbmUiIHN0cm9rZT0id2hpdGUiIHN0cm9rZS13aWR0aD0iMiIvPjwvc3ZnPg==)](https://ring-lang.github.io/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20|%20macOS%20|%20FreeBSD-green.svg?style=for-the-badge)](#)
[![Version](https://img.shields.io/badge/Version-1.0.0-orange.svg?style=for-the-badge)](#)

**Subprocess management library for the Ring programming language**

*Built on [subprocess.h](https://github.com/sheredom/subprocess.h) - A cross-platform process spawning library*

---

</div>

## Features

-   **Create Subprocesses**: Spawn child processes with full control over I/O
-   **I/O Redirection**: Read/write to subprocess stdin, stdout, stderr
-   **Environment Control**: Pass custom environment variables
-   **Cross-Platform**: Works on Windows, Linux, macOS, and FreeBSD
-   **High-Level API**: Simple `proc_shell()` and `proc_run()` for common use cases
-   **OOP Interface**: Clean object-oriented `Process` and `Shell` classes

## Installation

### Using RingPM

```bash
ringpm install proc from ysdragon
```

## Quick Start

```ring
load "proc.ring"

// Simple shell command
result = proc_shell("echo Hello World")
? result[2]  // Output: Hello World

// Using OOP interface
shell = new Shell
? shell.capture("whoami")
```

## Usage

### High-Level Functions

```ring
load "proc.ring"

// Shell command (uses system shell)
result = proc_shell("echo Hello World")
? "Return code: " + result[1]
? "Output: " + result[2]

// Run with arguments list
result = proc_run(["ls", "-la", "/tmp"], PROC_SEARCH_USER_PATH)
? "Return code: " + result[1]
? "Output: " + result[2]
? "Stderr: " + result[3]
```

### OOP Interface

```ring
load "proc.ring"

// Using Process class
proc = new Process(["cat"], PROC_SEARCH_USER_PATH)
proc.writeLine("Hello from Ring!")
proc.closeStdin()
proc.join()
? proc.stdout()
proc.destroy()

// Using Shell class
shell = new Shell
? shell.capture("pwd")              // Get output
? shell.success("test -f file.txt") // Check if command succeeded
```

### Manual Process Control

```ring
proc = proc_create(["cat"], PROC_SEARCH_USER_PATH)

proc_write(proc, "Hello World!" + nl)
proc_close_stdin(proc)

returnCode = proc_join(proc)
output = proc_stdout(proc)

? "Output: " + output

proc_destroy(proc)
```

### Custom Environment

```ring
proc = proc_create_ex(
    ["printenv", "MY_VAR"],
    PROC_SEARCH_USER_PATH,
    ["MY_VAR=HelloFromRing"]
)
proc_join(proc)
? proc_stdout(proc)
proc_destroy(proc)
```

## API Reference

### High-Level Functions

| Function | Description |
|----------|-------------|
| `proc_shell(cCommand [, nOptions])` | Run a shell command string. Returns `[returnCode, stdout, stderr]` |
| `proc_run(aCommandLine [, nOptions])` | Run command with arguments. Returns `[returnCode, stdout, stderr]` |

### Process Creation

| Function | Description |
|----------|-------------|
| `proc_create(aCommandLine [, nOptions])` | Create a subprocess |
| `proc_create_ex(aCommandLine, nOptions, aEnvironment)` | Create subprocess with custom environment |

### I/O Operations

| Function | Description |
|----------|-------------|
| `proc_write(pProcess, cData)` | Write data to subprocess stdin |
| `proc_close_stdin(pProcess)` | Close stdin (signal EOF) |
| `proc_read_stdout(pProcess, nMaxBytes)` | Read from stdout (async mode) |
| `proc_read_stderr(pProcess, nMaxBytes)` | Read from stderr (async mode) |
| `proc_stdout(pProcess)` | Read all remaining stdout |
| `proc_stderr(pProcess)` | Read all remaining stderr |

### Process Control

| Function | Description |
|----------|-------------|
| `proc_join(pProcess)` | Wait for process to finish, returns exit code |
| `proc_alive(pProcess)` | Check if process is still running |
| `proc_terminate(pProcess)` | Kill the subprocess |
| `proc_destroy(pProcess)` | Free process resources |

### Options Constants

| Constant | Description |
|----------|-------------|
| `PROC_COMBINED_STDOUT_STDERR` | Combine stdout and stderr |
| `PROC_INHERIT_ENVIRONMENT` | Inherit parent's environment |
| `PROC_ENABLE_ASYNC` | Enable async read operations |
| `PROC_NO_WINDOW` | No visible window (Windows) |
| `PROC_SEARCH_USER_PATH` | Search PATH for executable |

### OOP Classes

#### Process Class

```ring
proc = new Process(aCommand, nOptions)
proc.initWithEnv(aCommand, nOptions, aEnv)  // Alternative with environment
proc.write(cData)                            // Write to stdin
proc.writeLine(cData)                        // Write line to stdin
proc.closeStdin()                            // Close stdin
proc.readStdout(nMaxBytes)                   // Async read stdout
proc.readStderr(nMaxBytes)                   // Async read stderr
proc.stdout()                                // Get all stdout
proc.stderr()                                // Get all stderr
proc.join() / proc.wait()                    // Wait for completion
proc.isAlive()                               // Check if running
proc.terminate() / proc.kill()               // Kill process
proc.destroy()                               // Free resources
proc.exitCode()                              // Get exit code
```

#### Shell Class

```ring
shell = new Shell
shell.run(cCommand, nOptions)      // Run shell command, returns [code, out, err]
shell.exec(aCommand, nOptions)     // Run with args, returns [code, out, err]
shell.capture(cCommand)            // Run and return trimmed output
shell.captureLines(cCommand)       // Run and return output as list
shell.exitCode(cCommand)           // Run and return exit code
shell.success(cCommand)            // Run and return true if exit code is 0
```

## 🛠️ Development

If you wish to contribute to the development of Ring Proc or build it from the source, follow these steps.

### Prerequisites

-   **CMake**: Version 3.16 or higher.
-   **C Compiler**: A C compiler compatible with your platform (e.g., GCC, Clang, MSVC).
-   **Ring Source Code**: You will need to have the Ring language source code available on your machine.

### Build Steps

1.  **Clone the Repository:**
    ```sh
    git clone https://github.com/ysdragon/proc.git
    ```
    > **Note**: If you installed the library via RingPM, you can skip this step.

2.  **Set the `RING` Environment Variable:**
    This variable must point to the root directory of the Ring language source code.

    -   **Windows (Command Prompt):**
        ```cmd
        set RING=X:\path\to\ring
        ```
    -   **Windows (PowerShell):**
        ```powershell
        $env:RING = "X:\path\to\ring"
        ```
    -   **Unix-like Systems (Linux, macOS or FreeBSD):**
        ```bash
        export RING=/path/to/ring
        ```

3.  **Configure with CMake:**
    Create a build directory and run CMake from within it.
    ```sh
    mkdir build
    cd build
    cmake ..
    ```

4.  **Build the Project:**
    Compile the source code using the build toolchain configured by CMake.
    ```sh
    cmake --build .
    ```

    The compiled library will be available in the `lib/<os>/<arch>` directory.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
