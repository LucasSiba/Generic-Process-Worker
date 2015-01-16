#ifndef __GENERIC_PROCESS_WORKER_H__
#define __GENERIC_PROCESS_WORKER_H__

#define MAX_MSG_SIZE 1024

int   gpw_prepare(int argc, char ** argv);
void  gpw_next(char *buf, int buf_size);
void  gpw_run(void* run_data);

#endif
