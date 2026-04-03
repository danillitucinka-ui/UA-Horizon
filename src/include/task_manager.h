#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "stdint.h"

#define MAX_TASKS 32
#define MAX_NOTIFICATIONS 16
#define MAX_NAME_LEN 32

typedef enum {
    TASK_READY = 0,
    TASK_RUNNING = 1,
    TASK_WAITING = 2,
    TASK_COMPLETED = 3,
    TASK_FAILED = 4,
    TASK_SUSPENDED = 5
} task_state_t;

typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_NORMAL = 1,
    PRIORITY_HIGH = 2,
    PRIORITY_CRITICAL = 3
} task_priority_t;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    task_state_t state;
    task_priority_t priority;
    uint32_t created_at;
    uint32_t started_at;
    uint32_t completed_at;
    int exit_code;
    void (*entry)(void);
    void *stack;
    uint32_t stack_size;
    int is_system;
} task_t;

typedef struct {
    int id;
    char message[64];
    uint32_t timestamp;
    int is_read;
    int from_task_id;
} notification_t;

typedef struct {
    void (*callback)(void);
    uint32_t interval_ms;
    int is_active;
} scheduler_callback_t;

void task_manager_init(void);
int task_create(const char *name, void (*entry)(void), task_priority_t priority, uint32_t stack_size);
int task_start(int task_id);
int task_stop(int task_id);
int task_kill(int task_id);
int task_suspend(int task_id);
int task_resume(int task_id);
int task_set_priority(int task_id, task_priority_t priority);
task_t* task_get(int task_id);
task_t* task_get_current(void);
int task_get_count(void);
void task_yield(void);
void task_delay(uint32_t ms);

void scheduler_init(void);
void scheduler_run(void);
void scheduler_setquantum(uint32_t ms);
int scheduler_add_periodic(void (*callback)(void), uint32_t interval_ms);
int scheduler_remove_periodic(int id);
void scheduler_sleep_until(uint32_t wake_time);

void notification_init(void);
int notification_send(int task_id, const char *message);
int notification_get_count(void);
notification_t* notification_get(int index);
int notification_get_unread_count(void);
void notification_mark_read(int notification_id);
void notification_clear_all(void);

void task_manager_print_status(void);
int task_manager_get_cpu_usage(void);

// Global tick counter (updated by scheduler)
extern uint32_t tick_count;

#endif