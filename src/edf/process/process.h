#include <sys/types.h>
#include <stdint.h>

typedef enum status {
    RUNNING,
    READY,
    WAITING,
    FINISHED
} Status;

typedef struct process {
    pid_t pid;
    char name[32];
    float priority;
    uint32_t arrival_time;
    uint32_t deadline;
    Status status;
    uint32_t burst_time[255];
    uint32_t waiting_time[255];
    uint32_t burst_time_len;
} Process;


Process* process_init(pid_t pid, 
                      char* name, 
                      uint32_t arrival_time, 
                      uint32_t deadline, 
                      uint32_t* burst_time, 
                      uint32_t* waiting_time,
                      uint32_t burst_time_len
                      );

void process_destroy(Process* process);

void process_print(Process* process);