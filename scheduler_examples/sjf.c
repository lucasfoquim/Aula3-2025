#include "sjf.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"

void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {

    if (*cpu_task != NULL) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;

        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) {
                perror("write");
            }
            free(*cpu_task);
            *cpu_task = NULL;
        }
    }

    if (*cpu_task == NULL && rq->head != NULL) {
        queue_elem_t *min_elem = rq->head;
        queue_elem_t *min_prev = NULL;
        queue_elem_t *prev = NULL;
        queue_elem_t *curr = rq->head;

        while (curr != NULL) {
            if (curr->pcb->time_ms < min_elem->pcb->time_ms) {
                min_elem = curr;
                min_prev = prev;
            }
            prev = curr;
            curr = curr->next;
        }
        if (min_prev == NULL) {
            rq->head = min_elem->next;
        } else {
            min_prev->next = min_elem->next;
        }

        if (min_elem == rq->tail) {
            rq->tail = min_prev;
        }

        min_elem->next = NULL;

        *cpu_task = min_elem->pcb;
        free(min_elem);
    }
}
