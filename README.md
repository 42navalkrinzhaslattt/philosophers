# Philosophers

The dining philosophers problem in C, solved twice: with threads and mutexes, and with processes and POSIX semaphores. Instead of letting philosophers race for forks, both versions split them into two or three groups and rotate which group may eat, so neighbours never compete for the same fork.

**Stack:** C · POSIX threads · POSIX semaphores · Make    **Context:** 42 Berlin curriculum

## Table of Contents

- [Overview](#overview)
- [The Problem](#the-problem)
- [Build](#build)
- [Usage](#usage)
- [How It Works](#how-it-works)
- [Project Structure](#project-structure)
- [Testing](#testing)

## Overview

Two independent programs, each built as `philo` in its own directory:

- **Mandatory part (`philo/`):** one thread per philosopher, one mutex per fork, a monitor thread per philosopher and a scheduler thread
- **Bonus part (`philo_bonus/`):** one process per philosopher, all forks in one counting semaphore, a scheduler process, and the parent process killing everyone when the simulation ends

## The Problem

`N` philosophers sit at a round table with one fork between each pair of neighbours. Each one repeatedly eats, sleeps and thinks:

- **Eating** requires both adjacent forks
- A philosopher dies if `time_to_die` ms pass since the start of their last meal (or the start of the simulation)
- Philosophers don't communicate with each other
- The simulation stops when a philosopher dies or, if the optional last argument is given, when everyone has eaten that many times

## Build

Requires a POSIX system (Linux or macOS) and `cc`.

```bash
cd philo && make          # threads + mutexes  -> philo/philo
cd philo_bonus && make    # processes + semaphores -> philo_bonus/philo
```

```bash
make clean    # remove object files
make fclean   # remove object files and the binary
make re       # fclean + all
```

Compiled with `-Wall -Wextra -Werror`.

## Usage

```bash
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

| Parameter | Description |
|-----------|-------------|
| `number_of_philosophers` | Number of philosophers (and forks), at least 1 |
| `time_to_die` | Time in ms a philosopher can go without starting a meal |
| `time_to_eat` | Time in ms a philosopher spends eating |
| `time_to_sleep` | Time in ms a philosopher spends sleeping |
| `number_of_times_each_philosopher_must_eat` | Optional: stop once each philosopher has eaten this many times |

Arguments must be plain non-negative integers; anything else prints `philo: Invalid arguments` to stderr.

```bash
./philo 5 800 200 200       # runs until someone dies
./philo 5 800 200 200 7     # stops after every philosopher has eaten 7 times
./philo 1 800 200 200       # a single philosopher has one fork and dies
```

### Output format

Each state change is printed as `<ms since start> <philosopher> <action>`:

```
0 1 is thinking
0 1 has taken a fork
0 1 has taken a fork
0 1 is eating
200 1 is sleeping
...
```

Lines are indented with one tab per group number (none for group 0, one for group 1, two for group 2), which makes the eating rotation easy to see in the terminal.

## How It Works

### Eating groups

At start-up the philosophers are split into groups such that no two neighbours share a group:

- **Even `N`:** two groups, alternating seats (1, 3, 5, ... and 2, 4, 6, ...)
- **Odd `N`:** three groups: philosopher 1 alone, the remaining even seats, and the remaining odd seats

Only one group eats at a time. Group `g` starts eating at `g * time_to_eat`, and every philosopher repeats on a fixed period of

```
round_len = time_to_eat + max((groups - 1) * time_to_eat, time_to_sleep)
```

so the schedule is planned up front instead of emerging from contention.

### Mandatory part (threads + mutexes)

- **Philosopher threads** wait (polling every 0.5 ms) until their group is the active one, lock both forks, eat, increment their group's counter, then sleep and think.
- **Fork order:** even-numbered philosophers lock their right fork first, odd-numbered ones their left, as a second guard against circular waiting.
- **Scheduler thread (`controler`)** advances the active group once every member of the current group has eaten, then sleeps for about one meal (`time_to_eat`, or `time_to_sleep` at the end of a round when sleeping dominates the period) before checking again.
- **Monitor threads (`beholder`)**, one per philosopher, check every 0.5 ms whether `time_to_die` has passed since that philosopher's last meal. On a death the monitor prints `died` and sets a `funeral` flag on every philosopher under a mutex; all threads check it and stop, and logging is suppressed after the death message.
- Shared state is protected by separate mutexes: one per fork, one per philosopher for meal time and count, one per group counter, plus `write`, `death` and `flag` for output, the stop flag and the active group.

### Bonus part (processes + semaphores)

- **Philosopher processes:** each philosopher is a `fork()`ed child with its own detached monitor thread.
- **Forks:** a single named semaphore initialised to `N`; a philosopher takes two units before eating.
- **Scheduler process:** posts the active group's semaphore once per member, waits `time_to_eat`, collects the posts back, and moves to the next group. With a meal limit it stops after enough rounds and signals the end.
- **Termination:** a `death` semaphore is posted when a philosopher dies or the meal limit is reached; the parent waits on it and kills all philosopher processes and the scheduler with `SIGKILL`.
- Named semaphores are `sem_unlink`ed before creation, so leftovers from an earlier crashed run don't block start-up, and again at cleanup.

### Timing

`ft_usleep` sleeps in 0.5 ms steps and checks `gettimeofday` after each one, which avoids the overshoot of a single long `usleep`.

## Project Structure

```
philosophers/
├── philo/                    # mandatory part (threads + mutexes)
│   ├── Makefile
│   ├── includes/philo.h
│   └── srcs/
│       ├── philo.c           # main, thread start-up, monitor, ft_usleep
│       ├── routine.c         # scheduler, philosopher routine, eating
│       ├── parse.c           # argument parsing, group and philosopher setup
│       └── utils.c           # logging, errors, atoi, time, cleanup
└── philo_bonus/              # bonus part (processes + semaphores)
    ├── Makefile
    ├── includes/philo.h
    └── srcs/
        ├── philo.c           # main, child processes, monitor, kill on death
        ├── routine.c         # scheduler process, philosopher routine
        ├── parse.c           # parsing, semaphore setup, ft_usleep
        └── utils.c           # logging, errors, cleanup
```

## Testing

Standard evaluation cases from the subject, with the expected outcome:

```bash
# Nobody should die
./philo 5 800 200 200
./philo 4 410 200 200

# A philosopher should die
./philo 1 800 200 200
./philo 4 310 200 100

# Each philosopher eats 7 times, then the program exits
./philo 5 800 200 200 7

# Larger tables
./philo 100 800 200 200
./philo 200 800 200 200
```

To look for data races in the mandatory part, add `-g -fsanitize=thread` to `CFLAGS` in `philo/Makefile` (it is already there, commented out).

---

42 Berlin project by Nam Lap Vu ([42navalkrinzhaslattt](https://github.com/42navalkrinzhaslattt)).
