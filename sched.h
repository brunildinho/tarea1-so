/*
 * sched.h
 *
 *  Created on: May 26, 2014
 *      Author: xavier
 */

#ifndef SCHED_H_
#define SCHED_H_

#ifndef SCHED_C
int BatchResched(task_t tasks,int current_task, unsigned n);
unsigned RunBatch(task_t tasks, int n, unsigned *idle);
int TimeShareResched(task_t tasks,int current_task, unsigned n);
unsigned RunTimeShare(task_t tasks, unsigned quantum, unsigned n, unsigned *idle);
#endif

#endif /* SCHED_H_ */
