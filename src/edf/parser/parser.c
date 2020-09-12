#include "parser.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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
    printf("%s", line);
  }
  
  fclose(file);
}

// void parse_line(char* line)
// {

// }

// int main(void)
// {
//   read_file("test.txt");
//   return 0;
// }