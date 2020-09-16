#include "cpu.h"

CPU* cpu_init()
{
  CPU* cpu = malloc(sizeof(CPU));
  cpu -> time_left = 0;
  cpu -> busy = false;
  cpu->time_process_finished = 0;
  return cpu;
}

void cpu_destroy(CPU* cpu)
{
  free(cpu);
}