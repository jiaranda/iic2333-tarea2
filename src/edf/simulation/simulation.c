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

Process* get_next_process(Simulation* simulation, Status status)
{
  Process* process = NULL;
  uint32_t process_time;
  for (uint32_t i = 0; i < simulation->queue->process_qty; i++)
  {
    Process* current_process = simulation->queue->process_array[i];
    int32_t current_process_time = current_process->status_times[status];
    if (current_process->status == status)
    {
      if (!process)
      {
        process = current_process;
        process_time = current_process->status_times[status];
      }
      else if (current_process_time < process_time)
      {
        process = current_process;
        process_time = current_process->status_times[status];
      }
      else if (current_process_time == process_time && current_process->pid < process->pid)
      {
        process = current_process;
        process_time = current_process->status_times[status];
      }
    }
    else if (status == DEADLINE && current_process->status == READY)
    {
      if (!process)
      {
        process = current_process;
        process_time = current_process->status_times[status];
      }
      else if (current_process_time < process_time)
      {
        process = current_process;
        process_time = current_process->status_times[status];
      }
      else if (current_process_time == process_time && current_process->pid < process->pid)
      {
        process = current_process;
        process_time = current_process->status_times[status];
      }
    }
  }
  return process;
}

void add_process_to_cpu(Simulation* simulation)
{
  Process* next_deadline_process = get_next_process(simulation, DEADLINE);
  if (!next_deadline_process) return;

  CPU* cpu_free = NULL;
  uint32_t time_process_finished;
  for (uint32_t i = 0; i < simulation->CPU_qty; i++)
  {
    CPU* current_cpu = simulation->cpu_array[i];
    uint32_t current_cpu_time_process_finished = simulation->cpu_array[i]->time_process_finished;
    if (!cpu_free && !current_cpu->busy)
    {
      cpu_free = current_cpu;
      time_process_finished = current_cpu_time_process_finished;
    }
    else if (!current_cpu->busy && current_cpu_time_process_finished < time_process_finished)
    {
      cpu_free = current_cpu;
      time_process_finished = current_cpu_time_process_finished;
    }
  }
  if (cpu_free)
  {
    simulation->current_time++;
    cpu_free->process = next_deadline_process;
    cpu_free->time_process_added = simulation->current_time;
    cpu_free->busy = true;
    cpu_free->time_left = next_deadline_process->burst_time[next_deadline_process->current_burst];
    next_deadline_process->status = RUNNING;
    cpu_free->process->CPU_count++;
    if (cpu_free->process->CPU_count == 1)
    {
      cpu_free->process->response_time = simulation->current_time - cpu_free->process->arrival_time;
    }
    printf("[ADD FREE CPU] PID: %d - added process to cpu\n", cpu_free->process->pid);
    
  }
  else
  {
    CPU* cpu = NULL;
    uint32_t deadline_cpu;
    for (uint32_t i = 0; i < simulation->CPU_qty; i++)
    {
      CPU* current_cpu = simulation->cpu_array[i];
      uint32_t deadline_current_cpu = simulation->cpu_array[i]->process->deadline;
      if (!cpu && current_cpu->busy)
      {
        cpu = current_cpu;
        deadline_cpu = deadline_current_cpu;
      }
      else if (current_cpu->busy && deadline_current_cpu < deadline_cpu)
      {
        cpu = current_cpu;
        deadline_cpu = deadline_current_cpu;
      }
    }
    if (next_deadline_process->deadline < cpu->process->deadline)
    {
      simulation->current_time++;
      Process* out_process = cpu->process;
      out_process->status = READY;
      out_process->burst_time[out_process->current_burst] = simulation->current_time - cpu->time_process_added;
      out_process->interrupt_count++;
      next_deadline_process->status = RUNNING;
      cpu->process = next_deadline_process;
      cpu->time_process_added = simulation->current_time;
      cpu->time_left = next_deadline_process->burst_time[next_deadline_process->current_burst];
      cpu->process->CPU_count++;
      if (cpu->process->CPU_count == 1)
      {
        cpu->process->response_time = simulation->current_time - cpu->process->arrival_time;
      }
      printf("[ADD BUSY CPU] PID: %d - added process to cpu\n", cpu->process->pid);
    }
  }       
}


void handle_next_arrival_process(Simulation* simulation, Process* process)
{
  printf("[ARRIVAL]: PID %d arrived (handle_next_arrival_process)\n", process->pid);
  process->status = READY;
  simulation->current_time = process->arrival_time;
  add_process_to_cpu(simulation);
}

void handle_next_ready_process(Simulation* simulation, Process* process)
{
  printf("[READY]: PID %d READY, finished WAITING (handle_next_ready_process)\n", process->pid);
  process->status = READY;
  simulation->current_time = process->status_times[WAITING];
  process->current_burst++;
  add_process_to_cpu(simulation);
}

void handle_next_finished_burst_process(Simulation* simulation, CPU* cpu)
{
  simulation->current_time += cpu->time_left;
  cpu->busy = false;
  cpu->time_process_finished = simulation->current_time;
  if (cpu->process->current_burst == cpu->process->burst_time_len)
  {
    printf("[FINISHED BURST]: PID %d finished burst, now FINISHED (handle_next_finished_burst_process)\n", cpu->process->pid);
    cpu->process->status = FINISHED;
    cpu->process->turnaround_time = simulation->current_time - cpu->process->arrival_time;
    cpu->process->waiting_total_time = cpu->process->turnaround_time - cpu->process->initial_burst_total_time;
  }
  else
  {
    printf("[FINISHED BURST]: PID %d finished burst, now WAITING (handle_next_finished_burst_process)\n", cpu->process->pid);
    cpu->process->status = WAITING;
    cpu->process->status_times[WAITING] = simulation->current_time + cpu->process->waiting_time[cpu->process->current_burst];
  }
  cpu->process->interrupt_count++;
  add_process_to_cpu(simulation);

}

void run(Simulation* simulation)
{
  while (1)
  {
    Process* next_arrival_process = get_next_process(simulation, NOT_ARRIVED);
    Process* next_ready_process = get_next_process(simulation, WAITING);
    CPU* next_finished_burst_cpu = get_next_finished_burst_cpu(simulation);
    
    int32_t next_event_times[3] = {-1, -1, -1};
    if (next_arrival_process)
    {
      next_event_times[PROCESS_ARRIVAL] = next_arrival_process->arrival_time;
    }
    if (next_ready_process)
    {
      next_event_times[PROCESS_READY] = next_ready_process -> waiting_time[next_ready_process -> current_burst] + next_ready_process -> started_waiting_time;
    }
    if (next_finished_burst_cpu)
    {
      next_event_times[BURST_FINISHED] = next_finished_burst_cpu -> time_left + next_finished_burst_cpu -> time_process_added;
    }
    int next_event_index = -1;
    for (int i = 0; i < 3; i++)
    {
      if (next_event_times[i] != -1)
      {
        if (next_event_index == -1)
        {
          next_event_index = i;
        }
        else if (next_event_times[i] < next_event_times[next_event_index])
        {
          next_event_index = i;
        }
      }
    }
    switch (next_event_index)
    {
    case PROCESS_ARRIVAL:
      handle_next_arrival_process(simulation, next_arrival_process);
      break;
    case PROCESS_READY:
      handle_next_ready_process(simulation, next_ready_process);
      break;
    case BURST_FINISHED:
      handle_next_finished_burst_process(simulation, next_finished_burst_cpu);
      break;
    default:
      // finish simulation
      printf("Terminada!\n");
      return;
      break;
    }
  };
  
}

void load_process_output(Process* process, FILE* file)
{
  
  char* name = process->name;
  uint32_t CPU_count = process->CPU_count;
  uint32_t interrupt_count = process->interrupt_count;
  uint32_t turnaround_time = process->turnaround_time;
  uint32_t response_time = process->response_time;
  uint32_t waiting_total_time = process->waiting_total_time;
  //bool process_finished_bf_deadline = process->process_finished_bf_deadline;
  bool process_finished_bf_deadline = true;
  fprintf(
    file, 
    "%s,%u,%u,%u,%u,%u,%d\n", 
    name,
    CPU_count,
    interrupt_count,
    turnaround_time,
    response_time,
    waiting_total_time,
    process_finished_bf_deadline
    );
}

void output_simulation(Simulation* simulation, char* output_path)
{
  FILE* file = fopen(output_path, "w");
  fprintf(file, "name,CPU_count,interruption_count,turnaround_time,response_time,waiting_time,deadline\n");
  for (uint32_t i = 0; i < simulation->queue->process_qty; i++)
  {
    Process* process = simulation->queue->process_array[i];
    load_process_output(process, file);
  }
  fclose(file);
  
}
