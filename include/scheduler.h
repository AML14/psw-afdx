#ifndef SCHEDULER_H_
#define SCHEDULER_H_

typedef struct timespec timespec_t;

long start_scheduler();

void *schedule(void (*action)(void *), void *data, long interval);

void *schedule_ts(void (*action)(void *), void *data, timespec_t *interval);

void unschedule(void *hdlr);

#endif /* SCHEDULER_H_ */
