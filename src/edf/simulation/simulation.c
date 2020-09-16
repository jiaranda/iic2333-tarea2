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
  // iterar sobre cada elemento de la cola y entregar el proceso ready que tenga menor deadline
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
    // asignar al tiro el proceso con deadline más corto
    simulation->current_time++;
    cpu_free->process = next_deadline_process;
    cpu_free->time_process_added = simulation->current_time;
    cpu_free->busy = true;
    cpu_free->time_left = next_deadline_process->burst_time[next_deadline_process->current_burst];
    next_deadline_process->status = RUNNING;
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
      next_deadline_process->status = RUNNING;
      cpu->process = next_deadline_process;
      cpu->time_process_added = simulation->current_time;
      cpu->time_left = next_deadline_process->burst_time[next_deadline_process->current_burst];
      printf("[ADD BUSY CPU]PID: %d - added process to cpu\n", cpu->process->pid);
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
  // TIEMPO ACTUAL
  // pasar proceso de cpu a WAITING / FINISHED según caso
  simulation->current_time += cpu->time_left;
  if (cpu->process->current_burst == cpu->process->burst_time_len)
  {
    printf("[FINISHED BURST]: PID %d finished burst, now FINISHED (handle_next_finished_burst_process)\n", cpu->process->pid);
    cpu->process->status = FINISHED;
    cpu->busy = false;
    cpu->time_process_finished = simulation->current_time;
    add_process_to_cpu(simulation);
  }
  else
  {
    printf("[FINISHED BURST]: PID %d finished burst, now WAITING (handle_next_finished_burst_process)\n", cpu->process->pid);
    cpu->process->status = WAITING;
    cpu->busy = false;
    cpu->time_process_finished = simulation->current_time;
    cpu->process->status_times[WAITING] = simulation->current_time + cpu->process->waiting_time[cpu->process->current_burst];
  }
}

void run(Simulation* simulation)
{
  while (1)
  {
    // Process* next_arrival_process = get_next_arrival_process(simulation);
    // Process* next_ready_process = get_next_ready_process(simulation);
    // CPU* next_finished_burst_cpu = get_next_finished_burst_cpu(simulation);
    Process* next_arrival_process = get_next_process(simulation, NOT_ARRIVED);
    Process* next_ready_process = get_next_process(simulation, WAITING);
    CPU* next_finished_burst_cpu = get_next_finished_burst_cpu(simulation);



    // get next event
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

