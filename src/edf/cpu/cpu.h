typedef struct cpu
{
  uint32_t time_left;
  Process* process;
} CPU;

CPU* cpu_init();
void cpu_destroy(CPU* cpu);
void cpu_set_process(CPU* cpu, Process* process); // setear proceso y time_left en cpu, cambiar estado de proceso a RUNNING
void cpu_finish_burst(CPU* cpu); // termino de ejecutar y saca al proceso, cambiar estado a WAITING/FINISHED