#include <stdio.h>
#include <stdlib.h>
#include "simulation/simulation.h"

int main(int argc, char* argv[])
{
  char* input_path;
  char* output_path;
  uint32_t CPU_qty;
  switch (argc)
  {
  case 3:
    input_path = argv[1];
    output_path = argv[2];
    CPU_qty = 1;
    break;
  case 4:
    input_path = argv[1];
    output_path = argv[2];
    CPU_qty = atoi(argv[3]);
    break;
  default:
    printf("Error cantidad de argumentos \n");
    return 0;
  }  
  Simulation* simulation = simulation_init(input_path, CPU_qty);
  
  run(simulation);
  // TODO: correr simulación
  // TODO: generar output
  output_simulation(simulation, output_path);
  simulation_destroy(simulation);
  return 0;
}