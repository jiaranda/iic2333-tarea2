#include "process.h"

Process* process_init(pid_t pid, 
                      char* name, 
                      uint32_t arrival_time, 
                      uint32_t deadline, 
                      uint32_t* burst_time, 
                      uint32_t* waiting_time,
                      uint32_t burst_time_len)
{
  Process* process = malloc(sizeof(Process));
  process->pid = pid;
  strcpy(process->name, name);
  process -> burst_time_len = burst_time_len;
  process -> arrival_time = arrival_time;
  process -> deadline = deadline;
  process -> status = NOT_ARRIVED;
  process -> priority = 1;

  for (uint32_t i = 0; i < burst_time_len-1; i++)
  {
    process -> burst_time[i] = burst_time[i];
    process -> waiting_time[i] = waiting_time[i];
  }
  process -> burst_time[burst_time_len-1] = burst_time[burst_time_len-1];
  return process;
}

void process_destroy(Process* process)
{
  free(process);
}

void process_print(Process* process)
{
  printf("PID: %d\n", process -> pid);
  printf("name: %s\n", process -> name);
  printf("priority: %lf\n", process -> priority);
  printf("arrival time: %d\n", process -> arrival_time);
  printf("deadline: %d\n", process -> deadline);
  printf("status: %d\n", process -> status);
  printf("burst time len: %d\n", process -> burst_time_len);
}