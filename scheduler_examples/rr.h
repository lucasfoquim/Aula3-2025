#ifndef RR_H
#define RR_H
#include <stdint.h>
#include "queue.h"

// Fatia de tempo do Round Robin (em milissegundos)
#define TIME_SLICE_MS 500

void rr_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task);

#endif
