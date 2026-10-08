// Defines tasks structures


struct run_s {
	unsigned int run;	///< The time before doing an IO.
	unsigned int io;	///< The IO duration. If 0 then end of task.
};

typedef struct run_s* run_t;

struct task_s {
	unsigned start;												///< The task arrival time
	unsigned duration;											///< The task duration (total)
	int utime;													///< The task user time
	int rtime;													///< The task elapsed time (respond time)
	run_t run;													///< Array of run_s
	unsigned nruns;												///< Number of struct run_s for the task
	unsigned index;												///< Index of the actual run
	unsigned io_time;											///< IO duration counter
	enum {ready, blocked, terminated, notready, running} state; ///< Tasks state
};

typedef struct task_s* task_t; ///< The task list
typedef enum{batch, timeshare} sched_t; ///< The execution mode

#ifndef TASKS_C
unsigned int ReadTasks(char *filename);
void DisplayTasks(task_t tasks, unsigned n);
unsigned CheckTaskReady(task_t tasks, unsigned time, unsigned n);
void UpdateTasks(unsigned time, unsigned n);
unsigned StillHaveTasks(task_t tasks, unsigned n);
#endif
