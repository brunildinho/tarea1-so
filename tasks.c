// variable globales

#define TASKS_C

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include "tasks.h"

/// \var unsigned int ntasks=0
/// \brief The number of tasks
unsigned int ntasks=0;
/// \var task_t tasks
/// \brief The set of tasks
task_t tasks;

//extern unsigned cpu_time;

/** \brief Read tasks from file.
*  File is formated as follow:
*  Number of tasks
*  arrival IO_NUMBER IO_TIME:IO_DURATION ...
*  \param[in] filename File name
*  \return The number of tasks
*/
unsigned int ReadTasks(char *filename)
{
	FILE *file;
	unsigned int n;
	unsigned int i,j;
	unsigned int start,duration,ios,sum;
	run_t run;
	unsigned int iot, iod;

	file = fopen(filename, "r");
	if (file == NULL)
	{
		perror("Error lectura tareas");
		exit(1);
	}
	fscanf(file, "%d\n",&n);
	tasks = (task_t)malloc(n*sizeof(struct task_s));
	for(i=0;i<n;i++)
	{
		sum = 0;
		fscanf(file,"%d %d %d ", &start, &duration, &ios);
		tasks[i].start = start;
		tasks[i].duration = duration;
		tasks[i].utime = 0;
		tasks[i].rtime = 0;
		tasks[i].nruns = ios+1;
		tasks[i].index = 0;
		tasks[i].io_time = 0;
		tasks[i].state = notready;
		run = (run_t)malloc(sizeof(struct run_s)*(ios+1));
		tasks[i].run = run;
		for(j=0; j<ios; j++)
		{
			fscanf(file, "%d:%d", &iot, &iod);
			run[j].run = iot;
			run[j].io = iod;
			sum += iot;
		}
		run[ios].run = duration - sum;
		run[ios].io = 0;
		fscanf(file, "\n");
	}
	fclose(file);
	return(n);
}

/**
 * \brief Display the task list as read from file
 * \param[in] tasks The tasks list
 * \param[in] n The number of tasks
 */
void DisplayTasks(task_t tasks, unsigned n)
{
	unsigned int i,j;

	for(i=0; i<n; i++)
	{
		printf("Task %d: start at: %d, duration: %d\n", i, tasks[i].start, tasks[i].duration);
		printf("\tIOs at: ");
		for(j=0; j<tasks[i].nruns; j++)
		{
			if (tasks[i].run[j].io == 0)
				printf("%d:END", tasks[i].run[j].run);
			else
				printf("%d:%d  ", tasks[i].run[j].run, tasks[i].run[j].io);
		}
		printf("\n");
	}
}

/**
 * \brief Checks for new tasks arrival.
 * Tasks have a start time
 * Put all the new tasks in ready state
 * \param[in] tasks The tasks list
 * \param[in] time The actual CPU time
 * \param[in] n The number of tasks
 */
unsigned CheckTaskReady(task_t tasks, unsigned time, unsigned n)
{
	unsigned i;
	unsigned int rdy=0;

	for (i=0; i<n; i++)
	{
		if (tasks[i].start == time)
		{
			tasks[i].state = ready;
			tasks[i].rtime = 0;
		}
		if ((tasks[i].state == ready) || (tasks[i].state == running)) rdy++;
	}
	return(rdy);
}

/*
 * \brief Check if there is still some task to run
 * Task that we still have to execute are in notready or blocked state
 * \param[in] tasks The tasks list
 * \param[in] n The number of tasks
 * \return The number of tasks to execute
 */

unsigned StillHaveTasks(task_t tasks, unsigned n)
{
	unsigned i;
	unsigned count=0;

	for(i=0; i<n; i++)
		if ((tasks[i].state == notready) || (tasks[i].state == blocked)) count++;
	return(count);
}

/**
 * \brief Update task statistics and IOs at each CPU cycle
 * Update the elapsed time of blocked and ready tasks, and simulate
 * IOs duration
 * \param[in] time The actual CPU time
 * \param[in] n The number of tasks
 */
void UpdateTasks(unsigned time, unsigned n)
{
	unsigned i;

	for(i=0; i<n; i++)
	{
		switch( tasks[i].state)
		{
		case ready:
			tasks[i].rtime++;
			break;
		case blocked:
			tasks[i].rtime++;
			tasks[i].run[tasks[i].index].io--;
			if (tasks[i].run[tasks[i].index].io == 0)
			{
				tasks[i].index++;
				tasks[i].state = ready;
			}
			break;
		case notready:
		case running:
		default: break;
		}
	}
}
