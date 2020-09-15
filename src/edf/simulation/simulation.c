#include "simulation.h"

Simulation* simulation_init(char* input_path, uint32_t CPU_qty)
{
  Simulation* simulation = malloc(sizeof(Simulation));
  simulation -> CPU_qty = CPU_qty;
  simulation -> current_time = 0;
  Queue* queue = read_file(input_path);
  simulation -> queue = queue;
  for (int i = 0; i < CPU_qty; i++)
  {
    simulation -> cpu_array[i] = cpu_init();
  }
  return simulation;
}

void simulation_destroy(Simulation* simulation)
{
  queue_destroy(simulation->queue);
  for (int i = 0; i < simulation -> CPU_qty; i++)
  {
    cpu_destroy(simulation -> cpu_array[i]);
  }
  free(simulation);
}

Process* get_next_arrival_process(Simulation* simulation)
{
  Process* process = NULL;
  
  for (uint32_t i = 0; i < simulation -> queue -> process_qty; i++)
  {
    Process* current_process = simulation -> queue -> process_array[i];
    if (current_process -> status == NOT_ARRIVED)
    {
      if (!process)
      {
        process = current_process;
      }
      else if (current_process -> arrival_time < process -> arrival_time)
      {
        process = current_process;
      }
      else if (current_process -> arrival_time == process -> arrival_time && current_process -> pid < process -> pid)
      {
        process = current_process;
      }
    }
  }
  return process;
}

Process* get_next_ready_process(Simulation* simulation)
{
  Process* process = NULL;
  uint32_t process_stop_waiting;
  
  for (uint32_t i = 0; i < simulation -> queue -> process_qty; i++)
  {
    Process* current_process = simulation -> queue -> process_array[i];
    uint32_t current_process_waiting_time = current_process -> waiting_time[current_process -> current_burst];
    uint32_t current_process_started_waiting_time = current_process -> started_waiting_time;
    uint32_t current_process_stop_waiting = current_process_started_waiting_time + current_process_waiting_time;
    if (current_process -> status == WAITING)
    {
      if (!process)
      {
        process = current_process;
        process_stop_waiting = current_process_stop_waiting;
      }
      else if (current_process_stop_waiting < process_stop_waiting)
      {
        process = current_process;
        process_stop_waiting = current_process_stop_waiting;
      }
      else if (current_process_stop_waiting == process_stop_waiting && current_process -> pid < process -> pid)
      {
        process = current_process;
        process_stop_waiting = current_process_stop_waiting;
      }
    }
  }
  return process;
}

CPU* get_next_finished_burst_cpu(Simulation* simulation)
{
  CPU* cpu = NULL;
  uint32_t cpu_finish_burst_time;
  for (uint32_t i = 0; i < simulation -> CPU_qty; i++)
  {
    CPU* current_cpu = simulation -> cpu_array[i];
    uint32_t current_cpu_finish_burst_time = current_cpu -> time_left + current_cpu -> time_process_added;
    if(current_cpu -> busy == true)
    {
      if (!cpu)
      {
        cpu = current_cpu;
        cpu_finish_burst_time = cpu -> time_left + cpu -> time_process_added;
      }
      else if (current_cpu_finish_burst_time < cpu_finish_burst_time)
      {
        cpu = current_cpu;
        cpu_finish_burst_time = cpu -> time_left + cpu -> time_process_added;
      }
      else if (current_cpu_finish_burst_time == cpu_finish_burst_time && current_cpu->process->pid < cpu->process->pid)
      {
        cpu = current_cpu;
        cpu_finish_burst_time = cpu -> time_left + cpu -> time_process_added;
      }
    }
  }
  return cpu;
}

void run(Simulation* simulation)
{
  Process* next_arrival_process = get_next_arrival_process(simulation);
  process_print(next_arrival_process);
  Process* next_ready_process = get_next_ready_process(simulation);
  process_print(next_ready_process);
}

