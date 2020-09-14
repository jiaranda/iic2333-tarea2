#include "../queue/queue.h"
#include <stdint.h>

Queue* read_file(char* path, uint32_t CPU_qty);
Process* parse_line(char* line);