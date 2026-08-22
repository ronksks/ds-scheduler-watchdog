# Scheduler & Watchdog — C Data Structures Library

A production-grade C library implementing a **task scheduler** and **watchdog timer** subsystem, built on robust data structures (priority queue, heap, doubly/singly linked lists, unique ID allocator, circular buffer). Designed for embedded and systems programming contexts requiring deterministic scheduling, liveness monitoring, and concurrency safety.

---

## Features

- **Task Scheduler**
  - Priority-based task scheduling using a binary heap-backed priority queue.
  - Support for periodic and one-shot tasks with configurable intervals.
  - O(log n) insertion/removal, O(1) peek of highest-priority ready task.
  - Deterministic tick-driven advancement suitable for real-time systems.

- **Watchdog Timer**
  - Configurable timeout with periodic "kick" API to detect task hangs.
  - Integration with scheduler for coordinated timeout handling.
  - Safe reset and expiry callbacks for system recovery.

- **Thread Safety & Concurrency**
  - All public APIs are reentrant; internal state guarded by clear ownership rules.
  - Designed for single-threaded tick loops or integration with an external mutex layer.
  - No dynamic allocation in critical paths; memory ownership is explicit.

- **Performance**
  - Scheduler insert/remove: O(log n)
  - Scheduler tick: O(k log n) for k due tasks
  - Watchdog operations: O(1)
  - Minimal overhead per tick; suitable for resource-constrained environments.

---

## System Architecture

```
+-------------------+        +------------------+
|   Application     |------->|   Scheduler API  |
+-------------------+        +------------------+
                                    |
                                    v
                          +-------------------+
                          | Priority Queue    |
                          | (binary heap)     |
                          +-------------------+
                                    |
                    +---------------+---------------+
                    |               |               |
                    v               v               v
              +-----------+  +------------+  +------------+
              | Task Mgr  |  | UID Alloc  |  | DList/SList|
              +-----------+  +------------+  +------------+

+-------------------+        +------------------+
|   Application     |------->|  Watchdog API    |
+-------------------+        +------------------+
                                    |
                                    v
                          +-------------------+
                          | Timeout Tracker   |
                          +-------------------+
```

- **Scheduler** maintains a priority queue of `task_t` objects, each with:
  - Unique ID (from `uid` allocator)
  - Priority, period, next-run tick
  - Function pointer and context
- **Watchdog** tracks a single timeout deadline, reset via `watchdog_kick()`.
- Dependencies: `dlist`, `slist`, `heap`, `pqueue`, `uid`, `cbuffer`, and basic `utils`.

---

## Directory Structure

```
.
├── include/          # Public headers
├── src/              # Implementations
├── tests/            # Unit tests (C, assert-based)
├── examples/         # Sample applications
├── docs/             # Architecture deep-dive
├── Makefile          # Build & test targets
├── README.md         # This file
└── .gitignore
```

---

## Build & Test

### Requirements

- GCC or Clang
- `make`
- POSIX environment (Linux/macOS/WSL)

### Build

```bash
make clean
make all
```

### Run Tests

```bash
make test
```

This compiles and runs all unit tests for scheduler, watchdog, and dependencies.

---

## Usage Example

```c
#include "scheduler.h"
#include "watchdog.h"

static void my_task(void *ctx) {
    // Task logic here
}

int main(void) {
    scheduler_t *sched = scheduler_create();
    watchdog_t wd;

    watchdog_init(&wd, 5000); // 5s timeout

    task_id_t tid;
    scheduler_add_task(sched, my_task, NULL, 10, 100, &tid); // priority=10, period=100ms

    while (1) {
        scheduler_tick(sched, current_tick_ms());
        if (watchdog_is_expired(&wd)) {
            // Handle timeout
            watchdog_reset(&wd);
        }
        // Kick watchdog periodically
        watchdog_kick(&wd);
    }

    scheduler_destroy(sched);
    return 0;
}
```

---

## API Overview

### Scheduler

- `scheduler_t *scheduler_create(void);`
- `void scheduler_destroy(scheduler_t *sched);`
- `int scheduler_add_task(scheduler_t *sched, task_func_t func, void *ctx, int priority, unsigned period_ms, task_id_t *out_id);`
- `void scheduler_remove_task(scheduler_t *sched, task_id_t id);`
- `void scheduler_tick(scheduler_t *sched, unsigned now_ms);`

### Watchdog

- `void watchdog_init(watchdog_t *wd, unsigned timeout_ms);`
- `void watchdog_kick(watchdog_t *wd);`
- `int watchdog_is_expired(const watchdog_t *wd);`
- `void watchdog_reset(watchdog_t *wd);`

---

## Design Notes

- **No hidden allocations** in hot paths; all memory is explicitly managed.
- **Deterministic behavior**: no recursion, no unbounded loops.
- **Test coverage**: each module has dedicated unit tests validating edge cases (empty structures, overflow, boundary priorities, UID exhaustion).

---

## License

MIT License — see LICENSE file.

---

## Author

**Ron Yalensky**  
Systems Programmer | Embedded & High-Performance C  
[GitHub](https://github.com/ronksks)

---

> This library is optimized for clarity, performance, and safety in systems-level code. Ideal for embedded firmware, RTOS-like schedulers, and liveness-critical applications.
