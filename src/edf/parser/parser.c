#include "parser.h"
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

  uint32_t process_qty = atoi(strtok(fgets(line, sizeof(line), file), "\n"));
  while (fgets(line, sizeof(line), file))
  {
    // TODO: parse line & process_init
    parse_line(line);
  }
  
  fclose(file);
}

Process* parse_line(char* line)
{
  char local_line[2048];
  strcpy(&local_line, line);
  char* token = strtok(local_line, " ");
  int col = 0;
  char* name;
  pid_t pid;
  uint32_t arrival_time;
  uint32_t deadline;
  uint32_t burst_n;
  uint32_t burst_time[255];
  uint32_t waiting_time[255];
  uint32_t burst_time_len = 0;
  uint32_t waiting_time_len = 0;

  while (token != NULL)
  {
    if (col == 0)
    {
      name = token;
    }
    else if (col == 1)
    {
      pid = atoi(token);
    }
    else if (col == 2)
    {
      arrival_time = atoi(token);
    }
    else if (col == 3)
    {
      deadline = atoi(token);
    }
    else if (col == 4)
    {
      burst_n = atoi(token);
    }
    else if (col % 2 != 0)
    {
      burst_time[burst_time_len] = atoi(token);
      burst_time_len++;
    }
    else if (col % 2 == 0)
    {
      waiting_time[waiting_time_len] = atoi(token);
      waiting_time_len++;
    }
    col++;
    token = strtok(NULL, " ");
  }
  return process_init(
    pid, 
    name, 
    arrival_time, 
    deadline, 
    &burst_time, 
    &waiting_time, 
    burst_time_len
    );
}