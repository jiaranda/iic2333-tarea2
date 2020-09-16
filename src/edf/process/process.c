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
  process -> current_burst = 0;

  for (uint32_t i = 0; i < burst_time_len-1; i++)
  {
    process -> burst_time[i] = burst_time[i];
    process -> waiting_time[i] = waiting_time[i];
  }
  process -> burst_time[burst_time_len-1] = burst_time[burst_time_len-1];

  process->status_times[NOT_ARRIVED] = arrival_time;
  process->status_times[READY] = -1; // tiempo en que empieza ready
  process->status_times[RUNNING] = -1; // ultimpo tiempo en que paso a RUNNING
  process->status_times[WAITING] = -1; // tiempo en que terminara de hacer waiting
  process->status_times[FINISHED] = -1; // tiempo en que el proceso termino (paso a FINISHED)
  process->status_times[DEADLINE] = deadline;

  process->initial_burst_total_time = 0;
  for (uint32_t i = 0; i < burst_time_len; i++)
  {
    process->initial_burst_total_time += burst_time[i];
  }
  // output_variables
  process->CPU_count = 0;
  process->interrupt_count = 0;
  process->turnaround_time = 0;
  process->response_time = 0;
  process->waiting_total_time = 0;
  process->process_finished_bf_deadline = false;
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