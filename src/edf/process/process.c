#include "process.h"
#include <string.h>
#include <stdlib.h>

Process* process_init(pid_t pid, 
                      char* name, 
                      uint32_t arrival_time, 
                      uint32_t deadline, 
                      Status status, 
                      uint32_t* burst_time, 
                      uint32_t* waiting_time,
                      uint32_t n)
{
  Process* process = malloc(sizeof(Process));
  process->pid = pid;
  strcpy(process->name, name);
  process->arrival_time = arrival_time;
  process->deadline = deadline;
  process->status = READY;

  for (uint32_t i = 0; i < n-1; i++)
  {
    process -> burst_time[i] = burst_time[i];
    process -> waiting_time[i] = waiting_time[i];
  }
  process -> burst_time[n-1] = burst_time[n-1];
  
  return process;
}

void process_destroy(Process* process)
{
  free(process);
}