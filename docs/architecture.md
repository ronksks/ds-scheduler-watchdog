# Architecture Deep-Dive

## Scheduler Design

The scheduler implements a **tick-based priority scheduler** using a binary heap-backed priority queue.

### Data Structures

- **task_t**: Represents a scheduled task
  - `uid_t id`: Unique identifier from UID allocator
  - `int priority`: Task priority (lower = higher priority)
  - `unsigned period_ms`: Task period (0 for one-shot)
  - `unsigned next_run_ms`: Next scheduled execution tick
  - `task_func_t func`: Function pointer
  - `void *ctx`: User context

- **scheduler_t**: Main scheduler state
  - `pqueue_t *pq`: Priority queue of tasks
  - `unsigned current_tick_ms`: Current system tick

### Algorithms

**Insert**: O(log n)
1. Create task_t with UID, priority, period
2. Insert into priority queue

**Tick**: O(k log n) where k = number of due tasks
1. Update current_tick_ms
2. While pq.min().next_run_ms <= current_tick_ms:
   - Remove min task
   - Execute task.func(task.ctx)
   - If periodic: update next_run_ms += period_ms, re-insert

**Remove**: O(log n)
1. Find task by ID (linear scan in worst case)
2. Remove from priority queue

## Watchdog Design

The watchdog implements a **simple timeout monitor**.

### Data Structures

- **watchdog_t**: Watchdog state
  - `unsigned timeout_ms`: Timeout duration
  - `unsigned last_kick_ms`: Last kick timestamp
  - `int armed`: Whether watchdog is active

### Algorithms

**Kick**: O(1)
1. Update last_kick_ms = current_tick_ms

**Is Expired**: O(1)
1. Return (current_tick_ms - last_kick_ms) > timeout_ms

## Thread Safety

- All modules are **reentrant** but not internally locked
- Designed for single-threaded tick loops
- For multi-threaded use, wrap API calls with external mutexes

## Memory Model

- No hidden allocations in hot paths
- All memory ownership is explicit
- UID allocator prevents use-after-free via unique IDs
