/*! \mainpage Batch processing and Time Share simulator
 *
 * \author Xavier Bonnaire
 * \copyright Universidad Técnica Federico Santa María - 2014
 */
#include <stdio.h>
#include <stdlib.h>
#include "tasks.h"
#include "time.h"
#include "sched.h"

#define VERSION "1.1"

extern task_t tasks;
extern unsigned quantum;

int main(int argc, char *argv[])
{
	unsigned n;
	unsigned runmode;
	unsigned time;
	unsigned idle=0;

	printf("Tasks simulator v%s: ", VERSION);

	runmode = atoi(argv[1]);
	switch (runmode)
	{
	case 1:
	  printf("BATCH PROCESSING\n\n");
	  n = ReadTasks(argv[2]);               // Read task file
	  DisplayTasks(tasks, n);               // Display available tasks
	  time = RunBatch(tasks, n, &idle);     // Run task in batch processing
	  break;
	case 2:
	  printf("TIME SHARING\n\n");
	  quantum = atoi(argv[2]);
	  n = ReadTasks(argv[3]);
	  DisplayTasks(tasks, n);
	  time = RunTimeShare(tasks,quantum,n, &idle);          // Run tasks in time sharing
	  break;
	}
	DisplayStats(tasks, n, time, idle);       // Display tasks statistics after execution
	return(0);
}

