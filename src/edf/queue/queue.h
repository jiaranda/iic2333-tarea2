#include "../process/process.h"
#include <stdint.h>

typedef struct queue
{
  uint32_t process_qty;
  uint32_t CPU_qty;
  Process* process_array[255]; 
  // TODO: manejo CPUs
} Queue;

Queue* queue_init (uint32_t process_qty, uint32_t CPU_qty);
void add_process_to_queue(Queue* queue, Process* process, uint32_t position);
void queue_destroy(Queue* queue);
void queue_print(Queue* queue);