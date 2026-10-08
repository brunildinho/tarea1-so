// gestión del tiempo

#define TIME_C

#include <stdio.h>
#include "tasks.h"
#include "sched.h"

unsigned cpu_time=0; 	///< The CPU virtual time
unsigned quantum;		///< The time slice used for time sharing
enum {true, false} cpu_end=false;	///< Terminates simulation?

/**
 * \brief Displays the tasks statistics when simulation ends: \n
 * - Total execution time
 * - CPU time
 * - Elapsed time (respond time)
 * \param[in] tasks The tasks list
 * \param[in] n The number of tasks
 * \param[in] total The global execution time
 * \param[in] idle The CPU idle time
 */
void DisplayStats( task_t tasks, unsigned n, unsigned total, unsigned idle)
{
	unsigned i;

	printf("\n\nTasks: %d, Total execution time: %d\n", n, total);
	printf("Total CPU idle time: %d (%2.2f%%)\n", idle, ((float)idle/(float)total)*100);
	for(i=0; i<n; i++)
	{
		printf("Task %d, CPU time: %d, Elapsed: %d\n", i, tasks[i].utime, tasks[i].rtime);
	}
}
