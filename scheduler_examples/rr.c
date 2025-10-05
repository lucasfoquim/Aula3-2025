#include "rr.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"

#define TIME_SLICE_MS 500

void rr_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    static uint32_t slice_elapsed = 0;

    if (*cpu_task != NULL) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        slice_elapsed += TICKS_MS;

        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            write((*cpu_task)->sockfd, &msg, sizeof(msg_t));

            free(*cpu_task);
            *cpu_task = NULL;
            slice_elapsed = 0;
        }
        else if (slice_elapsed >= TIME_SLICE_MS) {
            queue_elem_t *new_elem = malloc(sizeof(queue_elem_t));
            new_elem->pcb = *cpu_task;
            new_elem->next = NULL;

            if (rq->tail == NULL)
                rq->head = rq->tail = new_elem;
            else {
                rq->tail->next = new_elem;
                rq->tail = new_elem;
            }

            *cpu_task = NULL;
            slice_elapsed = 0;
        }
    }
    if (*cpu_task == NULL && rq->head != NULL) {
        queue_elem_t *elem = rq->head;
        rq->head = elem->next;

        if (rq->head == NULL) {
            rq->tail = NULL;
            elem->next = NULL;
        }
        *cpu_task = elem->pcb;
        free(elem);
    }
}
