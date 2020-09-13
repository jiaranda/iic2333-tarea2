#include "parser.h"
#include "../process/process.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void read_file(char* path)
{
  FILE* file = fopen(path, "r");
  if (!file)
  {
    printf("error leyendo el archivo\n");
    return;
  }
  char* line[2048];

  while (fgets(line, sizeof(line), file))
  {
    // TODO: parse line & process_init
    parse_line(line);
  }
  
  fclose(file);
}

void parse_line(char* line)
{
  char local_line[2048];
  strcpy(&local_line, line);
  char* token = strtok(local_line, " ");
  while (token != NULL)
  {
    printf("%s\n", token);
    token = strtok(NULL, " ");
  }
}