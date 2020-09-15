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
  }
  return process;
}

void add_process_to_cpu(Simulation* simulation)
{
  // iterar sobre cada elemento de la cola y entregar el proceso ready que tenga menor deadline
  Process* next_deadline_process = get_next_process(simulation, DEADLINE);
  
  
  // hacer esto si asigno a CPU
  next_deadline_process->current_burst++;
  next_deadline_process->status = RUNNING;
  // next_deadline_process->status_times[RUNNING] = 
  //simulation->cpu_array[]


  // Iterar sobre cada cpu y encontrar al proceso que le falta más por terminar ???
  // si es que puedo
  // hacer swap --> meter proceso neuvo a la cpu y cambiar atributos
  printf("added process to cpu\n");        
}


void handle_next_arrival_process(Simulation* simulation, Process* process)
{
  // cambiar el estado del proceso de NOT_ARRIVED a READY
  process->status = READY;
  simulation->current_time = process->arrival_time;
  // intentar meter proceso a CPU
  add_process_to_cpu(simulation);
  printf("handle_next_arrival_process\n");
}

void handle_next_ready_process(Simulation* simulation, Process* process)
{
  // cambiar el estado del proceso de WAITING a READY
  process->status = READY;
  // actualizar tiempo en el proceso
  process->status_times[READY] = process->status_times[WAITING]; //el tiempo en que paso a ready
  // actualizar current time simulacion
  simulation->current_time = process->status_times[WAITING];
  // intentar meter proceso a CPU
  add_process_to_cpu(simulation);
  printf("handle_next_ready_process\n");
}

void handle_next_finished_burst_process(Simulation* simulation, CPU* cpu)
{
  // TIEMPO ACTUAL
  // pasar proceso de cpu a WAITING / FINISHED según caso
  if (cpu->process->current_burst == cpu->process->burst_time_len)
  {
    cpu->process->status = FINISHED;
    
  }
  else
  {
    cpu->process->status = WAITING;
    // cpu->process->status_times[WAITING] = simulation->current_time + 
  }
  
  
  // TIEMPO ACTUAL
  // pasar proceso de cpu a WAITING / FINISHED según caso
  // actualizar tiempo en el proceso
  // proceso de cpu a null ?? ---> pasar busy a false
  // TIEMPO ACTUAL + 1
  // proceso meterlo a CPU
  // actualizar tiempo en el proceso
  // pasar proceso de WAITING / NOT_ARRIVED a RUNNING
  printf("handle_next_finished_burst_process\n");
}

void run(Simulation* simulation)
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
    next_event_times[0] = next_arrival_process->arrival_time;
  }
  if (next_ready_process)
  {
    next_event_times[1] = next_ready_process -> waiting_time[next_ready_process -> current_burst] + next_ready_process -> started_waiting_time;
  }
  if (next_finished_burst_cpu)
  {
    next_event_times[2] = next_finished_burst_cpu -> time_left + next_finished_burst_cpu -> time_process_added;
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
  case 0:
    printf("Evento: process arriving\n");
    break;
  case 1:
    printf("Evento: process ready\n");
    break;
  case 2:
  printf("Evento: CPU ready\n");
    break;
  default:
    // finish simulation
    printf("temrinada\n");
    break;
  }
}

