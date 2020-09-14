#include "queue.h"

Queue* queue_init (uint32_t process_qty, uint32_t CPU_qty)
{
  Queue* queue = malloc(sizeof(Queue));
  queue -> process_qty = process_qty;
  queue -> CPU_qty = CPU_qty;
};

void add_process_to_queue(Queue* queue, Process* process, uint32_t position)
{
  queue -> process_array[position] = process;
};

void queue_destroy(Queue* queue)
{
  for (int i = 0; i < queue -> process_qty; i++)
  {
    process_destroy(queue -> process_array[i]);
  }
  free(queue);
}

void queue_print(Queue* queue)
{
  for (int i = 0; i < queue -> process_qty; i++)
  {
    printf("-----------------------------------\n");
    process_print(queue -> process_array[i]);
  }
}