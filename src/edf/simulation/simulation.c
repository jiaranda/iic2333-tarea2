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

void run(Simulation* simulation)
{
  Process* next_arrival_process = get_next_arrival_process(simulation);
  process_print(next_arrival_process);
}

