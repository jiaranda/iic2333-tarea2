#include "cpu.h"

CPU* cpu_init()
{
  CPU* cpu = malloc(sizeof(CPU));
  cpu -> time_left = 0;
  cpu -> busy = false;
  return cpu;
}

void cpu_destroy(CPU* cpu)
{
  free(cpu);
}