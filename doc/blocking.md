# Blocking calls handling

## Use case

1. The process wants to start a blocking operation (`sleep`, disk operation...).
   Via an interrupt, it calls the corresponding function from the driver.
2. The driver launches the operation and keeps the pid of the process.
3. The driver calls `scheduler_wait_blocking`, with a callback to cancel the
   operation as parameter (see below). The process is stopped and marked as
   `WAITING`.
4. When the operation completes (usually via a hardware interrupt), the driver
   calls `scheduler_wake_blocking` with the pid and a return value as parameter.
   The process is marked as `READY`.  Its saved value for `A` is set to the
   return value.
5. The execution of the process can restart from the line after the call to
   `scheduler_wait_blocking`. The result code is the return value of
   `scheduler_wait_blocking`.

## Scheduler

The scheduler only launches processes in the `READY` state. If none are
available, it runs `HLT`.

## Process end

If a process is killed while `WAITING`, the scheduler calls the callback with
the pid as parameter.
