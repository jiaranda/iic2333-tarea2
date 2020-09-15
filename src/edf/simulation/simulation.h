#include "../parser/parser.h"
#include "../cpu/cpu.h"

typedef struct simulation
{
  CPU* cpu_array[255];
  Queue* queue;
  uint32_t current_time;
  uint32_t CPU_qty;
} Simulation;

Simulation* simulation_init(char* input_path, uint32_t CPU_qty);
void simulation_destroy(Simulation* simulation);
void run(Simulation* simulation);

uint32_t get_next_arrival_process(Simulation* simulation);
uint32_t get_next_ready_process(Simulation* simulation);
uint32_t get_next_finished_burst_process(Simulation* simulation);
void handle_next_arrival_process(Simulation* simulation);
void handle_next_ready_process(Simulation* simulation);
void handle_next_finished_burst_process(Simulation* simulation);