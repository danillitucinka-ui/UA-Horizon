#include "../../include/task_manager.h"
#include "../../include/stdio.h"

static task_t tasks[MAX_TASKS];
static int task_count = 0;
static int current_task_id = -1;
static uint32_t tick_count = 0;
static uint32_t scheduler_quantum = 10;
static int next_task_id = 0;

static notification_t notifications[MAX_NOTIFICATIONS];
static int notification_count = 0;

static scheduler_callback_t periodic_tasks[8];
static int periodic_count = 0;

void task_manager_init(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_READY;
        tasks[i].id = -1;
    }
    task_count = 0;
    current_task_id = -1;
    next_task_id = 0;
    tick_count = 0;
    
    task_create("Idle", 0, PRIORITY_LOW, 4096);
    printf("[TaskManager] Initialized with %d max tasks\n", MAX_TASKS);
}

int task_create(const char *name, void (*entry)(void), task_priority_t priority, uint32_t stack_size) {
    if (task_count >= MAX_TASKS) return -1;
    
    int tid = next_task_id++;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].id == -1) {
            tid = i;
            break;
        }
    }
    
    task_t *t = &tasks[tid];
    t->id = tid;
    t->state = TASK_READY;
    t->priority = priority;
    t->entry = entry;
    t->created_at = tick_count;
    t->started_at = 0;
    t->completed_at = 0;
    t->exit_code = 0;
    t->stack_size = stack_size;
    t->is_system = (entry == 0);
    
    int len = 0;
    while (name[len] && len < MAX_NAME_LEN - 1) {
        t->name[len] = name[len];
        len++;
    }
    t->name[len] = 0;
    
    task_count++;
    return tid;
}

int task_start(int task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return -1;
    if (tasks[task_id].id == -1) return -1;
    
    tasks[task_id].state = TASK_RUNNING;
    tasks[task_id].started_at = tick_count;
    return 0;
}

int task_stop(int task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return -1;
    if (tasks[task_id].id == -1) return -1;
    
    tasks[task_id].state = TASK_WAITING;
    return 0;
}

int task_kill(int task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return -1;
    if (tasks[task_id].id == -1) return -1;
    if (task_id == 0) return -1;
    
    tasks[task_id].state = TASK_FAILED;
    tasks[task_id].completed_at = tick_count;
    tasks[task_id].exit_code = -1;
    task_count--;
    
    char msg[64];
    copy_str(msg, "Task terminated: ");
    concat_str(msg, tasks[task_id].name);
    notification_send(task_id, msg);
    
    return 0;
}

int task_suspend(int task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return -1;
    if (tasks[task_id].id == -1) return -1;
    
    tasks[task_id].state = TASK_SUSPENDED;
    return 0;
}

int task_resume(int task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return -1;
    if (tasks[task_id].id == -1) return -1;
    
    tasks[task_id].state = TASK_READY;
    return 0;
}

int task_set_priority(int task_id, task_priority_t priority) {
    if (task_id < 0 || task_id >= MAX_TASKS) return -1;
    if (tasks[task_id].id == -1) return -1;
    
    tasks[task_id].priority = priority;
    return 0;
}

task_t* task_get(int task_id) {
    if (task_id < 0 || task_id >= MAX_TASKS) return 0;
    if (tasks[task_id].id == -1) return 0;
    return &tasks[task_id];
}

task_t* task_get_current(void) {
    return task_get(current_task_id);
}

int task_get_count(void) {
    return task_count;
}

void task_yield(void) {
    if (current_task_id == 0) return;
    
    for (int i = 1; i < MAX_TASKS; i++) {
        int next = (current_task_id + i) % MAX_TASKS;
        if (tasks[next].id != -1 && tasks[next].state == TASK_RUNNING) {
            current_task_id = next;
            if (tasks[next].entry) {
                tasks[next].entry();
            }
            break;
        }
    }
}

void task_delay(uint32_t ms) {
    uint32_t start = tick_count;
    while ((tick_count - start) < ms) {
        task_yield();
    }
}

void scheduler_init(void) {
    for (int i = 0; i < 8; i++) {
        periodic_tasks[i].is_active = 0;
    }
    periodic_count = 0;
    printf("[Scheduler] Initialized\n");
}

void scheduler_run(void) {
    tick_count++;
    
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].id != -1 && tasks[i].state == TASK_RUNNING) {
            if (tasks[i].priority >= PRIORITY_HIGH) {
                current_task_id = i;
                if (tasks[i].entry) {
                    tasks[i].entry();
                }
                break;
            }
        }
    }
    
    for (int i = 0; i < 8; i++) {
        if (periodic_tasks[i].is_active && tick_count % (periodic_tasks[i].interval_ms / 10) == 0) {
            periodic_tasks[i].callback();
        }
    }
    
    if (tick_count % (scheduler_quantum * 10) == 0) {
        task_yield();
    }
}

void scheduler_setquantum(uint32_t ms) {
    scheduler_quantum = ms;
}

int scheduler_add_periodic(void (*callback)(void), uint32_t interval_ms) {
    if (periodic_count >= 8) return -1;
    
    periodic_tasks[periodic_count].callback = callback;
    periodic_tasks[periodic_count].interval_ms = interval_ms;
    periodic_tasks[periodic_count].is_active = 1;
    periodic_count++;
    return periodic_count - 1;
}

int scheduler_remove_periodic(int id) {
    if (id < 0 || id >= 8) return -1;
    periodic_tasks[id].is_active = 0;
    return 0;
}

void scheduler_sleep_until(uint32_t wake_time) {
    while (tick_count < wake_time) {
        task_yield();
    }
}

void notification_init(void) {
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        notifications[i].id = -1;
    }
    notification_count = 0;
    printf("[Notifications] System initialized\n");
}

int notification_send(int task_id, const char *message) {
    if (notification_count >= MAX_NOTIFICATIONS) return -1;
    
    int nid = -1;
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        if (notifications[i].id == -1) {
            nid = i;
            break;
        }
    }
    if (nid == -1) return -1;
    
    notifications[nid].id = notification_count++;
    notifications[nid].timestamp = tick_count;
    notifications[nid].is_read = 0;
    notifications[nid].from_task_id = task_id;
    
    int len = 0;
    while (message[len] && len < 63) {
        notifications[nid].message[len] = message[len];
        len++;
    }
    notifications[nid].message[len] = 0;
    
    return nid;
}

int notification_get_count(void) {
    return notification_count;
}

notification_t* notification_get(int index) {
    if (index < 0 || index >= MAX_NOTIFICATIONS) return 0;
    if (notifications[index].id == -1) return 0;
    return &notifications[index];
}

int notification_get_unread_count(void) {
    int count = 0;
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        if (notifications[i].id != -1 && !notifications[i].is_read) {
            count++;
        }
    }
    return count;
}

void notification_mark_read(int notification_id) {
    if (notification_id < 0 || notification_id >= MAX_NOTIFICATIONS) return;
    notifications[notification_id].is_read = 1;
}

void notification_clear_all(void) {
    for (int i = 0; i < MAX_NOTIFICATIONS; i++) {
        notifications[i].id = -1;
    }
    notification_count = 0;
}

static void copy_str(char *dest, const char *src) {
    int i = 0;
    while (src[i] && i < 63) { dest[i] = src[i]; i++; }
    dest[i] = 0;
}

static void concat_str(char *dest, const char *src) {
    int i = 0;
    while (dest[i]) i++;
    int j = 0;
    while (src[j] && i < 63) { dest[i++] = src[j++]; }
    dest[i] = 0;
}

void task_manager_print_status(void) {
    printf("\n=== Task Manager Status ===\n");
    printf("Running tasks: %d\n", task_count);
    printf("Current task: %d\n", current_task_id);
    printf("Tick count: %u\n", tick_count);
    printf("\n");
    printf("ID   Name              State      Priority\n");
    printf("---- ----------------- ---------- --------\n");
    
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].id != -1) {
            const char *state_str = "UNKNOWN";
            const char *prio_str = "NORMAL";
            
            switch (tasks[i].state) {
                case TASK_READY: state_str = "READY"; break;
                case TASK_RUNNING: state_str = "RUNNING"; break;
                case TASK_WAITING: state_str = "WAITING"; break;
                case TASK_COMPLETED: state_str = "DONE"; break;
                case TASK_FAILED: state_str = "FAILED"; break;
                case TASK_SUSPENDED: state_str = "SUSPENDED"; break;
            }
            
            switch (tasks[i].priority) {
                case PRIORITY_LOW: prio_str = "LOW"; break;
                case PRIORITY_NORMAL: prio_str = "NORMAL"; break;
                case PRIORITY_HIGH: prio_str = "HIGH"; break;
                case PRIORITY_CRITICAL: prio_str = "CRITICAL"; break;
            }
            
            printf("%-4d %-17s %-10s %s\n", 
                   tasks[i].id, tasks[i].name, state_str, prio_str);
        }
    }
    
    int unread = notification_get_unread_count();
    printf("\nNotifications: %d unread\n", unread);
}

int task_manager_get_cpu_usage(void) {
    int running = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].id != -1 && tasks[i].state == TASK_RUNNING) {
            running++;
        }
    }
    return (running * 100) / (task_count > 0 ? task_count : 1);
}