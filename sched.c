/*
 * sched.c
 *
 *  Created on: May 26, 2014
 *      Author: xavier
 */

#include <stdio.h>
#include "tasks.h"

/**
 * \brief Choose next task in batch processing \n
 * The next task in ready state in chosen
 * \param[in] tasks The tasks list
 * \param[in] current_task The current_task index
 * \param[in] n The number of tasks
 */
int BatchResched(task_t tasks,int current_task, unsigned n)
{
	unsigned next;
	unsigned count=0;

	for(next=0; next<n; next++)
		if (tasks[next].state == ready) // This is the new task
		{
			tasks[next].state = running;
			return(next);
		} else if (tasks[next].state == notready) count++;
	if (count != 0) return(-2); // Some task are not yet ready
		else return(-1); // There is no more task
}

/**
 * \brief Runs the tasks list in batch processing \n
 * Each task is terminated before running the next one. There is
 * no preemption. \n
 * This function is executed at each CPU cycle
 * \param[in] tasks The tasks list
 * \param[in] current_task The task being executed
 * \param[in] idle The CPU idle time
 * \return The total running time
 */
unsigned RunBatch(task_t tasks, int n, unsigned *idle)
{
	unsigned time;
	unsigned i;
	unsigned current_task=0;
	unsigned rdy=0;
	unsigned index;

	time=0; // starting time;
	*idle = 0; // cpu idel time


	while(1)
	{
		rdy = CheckTaskReady(tasks, time, n);
		if (rdy > 0)
		{
			// Run the next task
			current_task = BatchResched(tasks, current_task, n);
			index = tasks[current_task].index;
			while( index < tasks[current_task].nruns)
			{
				switch (tasks[current_task].state)
				{
				case running:
					printf("%d",current_task);
					tasks[current_task].utime++;
					tasks[current_task].rtime++;
					tasks[current_task].run[index].run--;
					// updates every other task in ready state
					for(i=0; i<n; i++)
						if (i != current_task)
							if( tasks[i].state == ready) tasks[i].rtime++;
					if (tasks[current_task].run[index].run == 0)
					{
						if (tasks[current_task].run[index].io != 0)
						{
							// task start I/O
							tasks[current_task].state = blocked;
							printf("#");
							fflush(stdout);
						} else
							index++;
					}
					break;
				case blocked:
					printf("X");
					(*idle)++;
					tasks[current_task].rtime++;
					// updates every other task in ready state
					for (i = 0; i < n; i++)
						if (i != current_task)
							if (tasks[i].state == ready)
								tasks[i].rtime++;
					tasks[current_task].run[index].io--;
					if (tasks[current_task].run[index].io == 0)
					{
						// I/O is finished
						tasks[current_task].state = running;
						// next run
						index++;
					}
					break;
				default: break;
				}
				time++;
				rdy = CheckTaskReady(tasks, time, n);
			}
			// Task terminated;
			tasks[current_task].state = terminated;
			printf("*/");fflush(stdout);
		}
		else
		{
			// There is no task ready
			// Check if there are still task to run...
			current_task = BatchResched(tasks, current_task, n);
			if (current_task == -1) return(time);
			else time++;
		}

	}
	return(0);
}


/**
 * \brief Chose next task to execute (shechdule) in time sharing. \n
 * Uses a simple Round Robin policy without preemption, and without priorities.
 * The function is called at end of quantum or at IO
 * \param[in] tasks The tasks list
 * \param[in] current_task The current task in execution
 * \param[in] n The number of tasks
 * \return The next task to run
 * \return -1 If there is no more tasks
 * \return -2 If there are tasks in state notready or blocked and none is available to run
 */
int TimeShareResched(task_t tasks,int current_task, unsigned n)
{
	unsigned count=0;
	unsigned wait=0;
	int index;

	if (current_task == -2) index = -1;
		else index = current_task;

	while( count<n )
	{
		index = (index+1)%n;
		count++;
		if (tasks[index].state == ready)
		{
			tasks[index].state = running;
			return(index);
		}
		if ((tasks[index].state == blocked) || (tasks[index].state == notready)) wait++;
	}
	if (wait != 0) return(-2);
		else return(-1);
}


/**
 * \brief Runs the tasks in time sharing.\n
 * The function is called a each CPU cycle \n
 * \param[in] tasks The tasks list
 * \param[in] current_task The current running task
 * \param[in] quantum The time slice
 * \param[in] idle The CPU idle time
 * \return 1 If we need to re-schedule (the task ends quantum, IO, or ended)
 * \return 0 If the tasks still has the CPU
 */
unsigned RunTimeShare(task_t tasks, unsigned quantum, unsigned n, unsigned *idle)
{
	unsigned time;
	unsigned rdy;
	int current_task=-1;
	unsigned used_quantum;
	unsigned index;

	// Starts time
	time = 0;
	*idle = 0;

	while(1)
	{
		rdy = CheckTaskReady(tasks, time, n);
		if (rdy > 0)
		{
			// Run the next task
			current_task = TimeShareResched(tasks, current_task, n);
			used_quantum = 0;
			index = tasks[current_task].index;
			// Run task for at most one quantum
			while(used_quantum < quantum)
			{
				printf("%d", current_task);fflush(stdout);
				tasks[current_task].utime++;
				tasks[current_task].rtime++;
				tasks[current_task].run[index].run--;
				// updates every other task in ready or blocked states
				UpdateTasks(time, n);
				// Check if task reached I/O
				if (tasks[current_task].run[index].run == 0)
				{
					if (tasks[current_task].run[index].io != 0)
					{
						// task start I/O
						printf("#");fflush(stdout);
						tasks[current_task].state = blocked;
						time++;
						break; // exit from while and reschedule tasks (round robin)
					}
					else
					{
						// task ends and reschedule
						printf("*");fflush(stdout);
						tasks[current_task].state = terminated;
						time++;
						break;
					}
				}
				used_quantum++;
				time++;
				rdy = CheckTaskReady(tasks, time, n);
			}
			// task has used quantum or is in I/O
			printf("/");fflush(stdout);
			if (tasks[current_task].state == running) tasks[current_task].state = ready;
		}
		else
		{
			// Wait for tasks to be ready
			current_task = -1;
			if (!StillHaveTasks(tasks,n)) return(time);
			(*idle)++;
			printf("X"); fflush(stdout);
			UpdateTasks(time, n);
			time++;
			rdy = CheckTaskReady(tasks, time, n);
		}
	}
	return(0);
}
