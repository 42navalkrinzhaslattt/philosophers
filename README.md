# Philosophers

> *"I never thought philosophy would be so deadly"*

A classic concurrency problem implemented in C using threads and processes to simulate the dining philosophers problem.

## 📋 Table of Contents

- [Overview](#overview)
- [The Problem](#the-problem)  
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Implementation Details](#implementation-details)
- [Testing](#testing)
- [Common Issues](#common-issues)

## 🎯 Overview

This project explores the fundamentals of threading and process synchronization through the classic **Dining Philosophers Problem**. The implementation includes two versions:

- **Mandatory Part (`philo/`)**: Uses threads and mutexes
- **Bonus Part (`philo_bonus/`)**: Uses processes and semaphores

## 🍝 The Problem

Philosophers sit around a circular table with a bowl of spaghetti in the center. Each philosopher alternates between three activities:

- **🍴 Eating**: Requires two forks (left and right)
- **💭 Thinking**: Contemplating life's mysteries  
- **😴 Sleeping**: Resting after a meal

### Rules

1. Philosophers cannot communicate with each other
2. They don't know when others might die
3. A philosopher dies if they don't start eating within `time_to_die` milliseconds
4. To eat, a philosopher must acquire both adjacent forks
5. The simulation stops when a philosopher dies or all have eaten enough times

## 📁 Project Structure

```
philosophers/
├── README.md
├── philo/                    # Mandatory part (threads + mutexes)
│   ├── Makefile
│   ├── includes/
│   │   └── philo.h
│   └── srcs/
│       ├── philo.c           # Main program and initialization
│       ├── routine.c         # Philosopher routine and actions
│       ├── parse.c           # Argument parsing and validation
│       └── utils.c           # Utility functions
└── philo_bonus/              # Bonus part (processes + semaphores)
    ├── Makefile
    ├── includes/
    │   └── philo.h
    └── srcs/
        ├── philo.c           # Main program with processes
        ├── routine.c         # Process-based philosopher routine
        ├── parse.c           # Argument parsing and validation
        └── utils.c           # Utility functions
```

## ⚙️ Requirements

### System Requirements
- **OS**: Unix-based system (Linux, macOS)
- **Compiler**: `cc` with flags `-Wall -Wextra -Werror`
- **Standards**: C99 or later

### Allowed Functions

**Mandatory Part:**
- `memset`, `printf`, `malloc`, `free`, `write`
- `usleep`, `gettimeofday`
- `pthread_create`, `pthread_detach`, `pthread_join`
- `pthread_mutex_init`, `pthread_mutex_destroy`
- `pthread_mutex_lock`, `pthread_mutex_unlock`

**Bonus Part:**
- All mandatory functions plus:
- `fork`, `kill`, `exit`, `waitpid`
- `sem_open`, `sem_close`, `sem_post`, `sem_wait`, `sem_unlink`

## 🚀 Installation

### Mandatory Part (Threads)

```bash
cd philo
make
```

### Bonus Part (Processes)

```bash
cd philo_bonus
make
```

### Clean Build Files

```bash
make clean    # Remove object files
make fclean   # Remove object files and executable
make re       # Clean and rebuild
```

## 💻 Usage

### Command Syntax

```bash
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

### Parameters

| Parameter | Description |
|-----------|-------------|
| `number_of_philosophers` | Number of philosophers (and forks) |
| `time_to_die` | Time in ms before a philosopher dies without eating |
| `time_to_eat` | Time in ms a philosopher spends eating |
| `time_to_sleep` | Time in ms a philosopher spends sleeping |
| `number_of_times_each_philosopher_must_eat` | Optional: Stop after each philosopher eats this many times |

### Example Usage

```bash
# Basic simulation with 5 philosophers
./philo 5 800 200 200

# Simulation that stops after each philosopher eats 7 times
./philo 5 800 200 200 7

# Edge case: single philosopher (should die)
./philo 1 800 200 200

# Stress test with many philosophers
./philo 200 800 200 200
```

## 🔧 Implementation Details

### Mandatory Part (Threads + Mutexes)

- Each philosopher is a separate thread
- Each fork is protected by a mutex
- Shared data structures for simulation state
- Death checking runs in a separate monitoring thread

### Bonus Part (Processes + Semaphores)

- Each philosopher is a separate process  
- Forks represented as a semaphore with N permits
- Inter-process communication for death detection
- Main process manages child processes

### Output Format

All state changes are logged with timestamps:

```
142 1 has taken a fork
142 1 has taken a fork  
142 1 is eating
344 1 is sleeping
544 1 is thinking
642 2 has taken a fork
...
```

### Key Implementation Challenges

1. **Race Conditions**: Proper mutex/semaphore usage
2. **Deadlock Prevention**: Careful fork acquisition order
3. **Death Detection**: Timely monitoring without data races
4. **Clean Termination**: Proper cleanup of threads/processes

## 🧪 Testing

### Basic Test Cases

```bash
# Should not die
./philo 5 800 200 200
./philo 4 410 200 200

# Should die  
./philo 1 800 200 200
./philo 4 310 200 100

# Edge cases
./philo 2 800 200 200
./philo 100 800 200 200
```

### Performance Testing

```bash
# Stress test with many philosophers
./philo 200 800 200 200

# Quick death scenario
./philo 4 310 200 100
```

### Validation Checklist

- [ ] No philosopher dies unexpectedly
- [ ] Death detection within 10ms
- [ ] No data races (use thread sanitizer)
- [ ] No memory leaks (use valgrind)
- [ ] Proper cleanup on exit
- [ ] Correct timestamp formatting
- [ ] Message ordering consistency

## 🐛 Common Issues

### Data Races
- **Problem**: Unsynchronized access to shared variables
- **Solution**: Protect all shared data with mutexes/semaphores

### Deadlocks
- **Problem**: Circular waiting for resources
- **Solution**: Consistent fork acquisition order or timeout mechanisms

### Death Detection Delays
- **Problem**: Death not detected within 10ms
- **Solution**: Frequent monitoring with minimal sleep intervals

### Memory Leaks
- **Problem**: Unreleased mutexes or unjoined threads
- **Solution**: Proper cleanup in exit paths

### Timing Issues
- **Problem**: Incorrect use of `usleep()` or timing calculations
- **Solution**: Use `gettimeofday()` for accurate timestamps

## 📚 Learning Objectives

This project teaches:

- **Threading**: Creating and managing threads
- **Synchronization**: Mutexes, semaphores, and critical sections
- **Concurrency**: Race conditions, deadlocks, and their prevention
- **Process Management**: Fork, exec, and inter-process communication
- **System Programming**: Low-level timing and resource management

## 🏆 Evaluation Criteria

- **Functionality**: Correct simulation behavior
- **Thread Safety**: No data races or undefined behavior  
- **Resource Management**: Proper cleanup and no leaks
- **Error Handling**: Graceful handling of edge cases
- **Code Quality**: Clean, readable, and well-structured code
- **Performance**: Efficient resource usage

---

*"The unexamined life is not worth living." - Socrates*

But apparently, the unexamined spaghetti is quite deadly! 🍝💀
