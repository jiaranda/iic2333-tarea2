#include <stdio.h>
#include <stdlib.h>
#include "parser/parser.h"

int main(int argc, char* argv[])
{
  char* input_path;
  char* output_path;
  int cpu_number;
  switch (argc)
  {
  case 3:
    input_path = argv[1];
    output_path = argv[2];
    cpu_number = 1;
    break;
  case 4:
    input_path = argv[1];
    output_path = argv[2];
    cpu_number = atoi(argv[3]);
    break;
  default:
    printf("Error cantidad de argumentos \n");
    return 0;
  }
  read_file(input_path);
  return 0;
}