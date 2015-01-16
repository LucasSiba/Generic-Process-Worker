#include <mqueue.h>
#include <unistd.h>
#include <signal.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generic-process-worker.h"

mqd_t mq;

static void
run_producer(void)
{
    while (1) {
        char buf[MAX_MSG_SIZE];
        gpw_next(buf, (int)sizeof(buf));
        if (strlen(buf) == 0) {
            exit(-1);
        }
        mq_send(mq, buf, strlen(buf), 0);
    }
}

static void
run_consumer(void)
{
    while (1) {
        char buf[MAX_MSG_SIZE];
        int bytes_read = mq_receive(mq, buf, 1024, NULL);
        if (bytes_read < 1) {
            exit(-1);
        }
        gpw_run(buf);
    }
}

int
main(int argc, char ** argv)
{
    char mq_name[128];
    int x;

    int worker_count = gpw_prepare(argc, argv);
    worker_count++; // + 1 for the producer

    struct mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 50;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    snprintf(mq_name, sizeof(mq_name), "/MQ-%d", getpid());
    mq = mq_open(mq_name, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == -1) {
        fprintf(stderr, "failed to create mq_open: %s\n", strerror(errno));
        exit(-1);
    }

    for (x = 0; x < worker_count; x++) {
        pid_t childpid;
        childpid = fork();
        if (childpid >= 0) {
            if (childpid == 0) { // Child
                if (x == 0) {
                    fprintf(stderr, "Producer PID: %d\n", getpid());
                    run_producer();
                } else {
                    fprintf(stderr, "Consumer PID: %d\n", getpid());
                    run_consumer();
                }
            }
            else { // Parent
                continue;   
            }
        } else {
            perror("fork");
            exit(-1); 
        }
    }

    // Find a cleaner way to exit!
    int status;
    pid_t done = wait(&status);
    fprintf(stderr, "PID %d exited!\n", done);
    kill(0, SIGKILL); // Kill process group

    return 0;
}
