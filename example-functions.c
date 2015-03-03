#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pem.h>

#include "generic-process-worker.h"

char pass_buf[MAX_MSG_SIZE - 1]; // Null terminated

#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 8
#endif
 
static void hexdump(void *mem, unsigned int len)
{
    unsigned int i, j;
    
    for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++) {
        /* print offset */
        if(i % HEXDUMP_COLS == 0) {
                printf("0x%06x: ", i);
        }

        /* print hex data */
        if(i < len) {
                printf("%02x ", 0xFF & ((char*)mem)[i]);
        }
        else /* end of block, just aligning for ASCII dump */ {
                printf("   ");
        }
        
        /* print ASCII dump */
        if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1)) {
                for(j = i - (HEXDUMP_COLS - 1); j <= i; j++) {
                        if(j >= len) /* end of block, not really printing */ {
                                putchar(' ');
                        }
                        else if(isprint(((char*)mem)[j])) /* printable char */ {
                                putchar(0xFF & ((char*)mem)[j]);        
                        }
                        else /* other char */ {
                                putchar('.');
                        }
                }
                putchar('\n');
        }
    }
}

// Called before anything else
// Return the number of process workers to start
int
gpw_prepare(int argc, char ** argv)
{
    OpenSSL_add_all_algorithms();

    int worker_count = 1;
    if (argc != 1) {
        worker_count = atoi(argv[1]);
    }

    memset(pass_buf, 0, sizeof(pass_buf));
    return worker_count;
}

// This function will eventually overflow itself!
static void
ascii_increment(char *buf)
{
AGAIN:
    (*buf)++;
    if (*buf == '\0') {
        *buf = ' ';
        ascii_increment(buf + 1);
    } else if (!isprint(*buf)) {
        goto AGAIN;
    }
    return;
}

// Prepare the next peice of data for work
// Return NULL means stop
void
gpw_next(char* buf, int buf_size)
{
    static unsigned counter = 0;
    if (counter++ % 100000 == 0) {
        //printf("\nlen = %d\n", (int)strlen(pass_buf));
        hexdump(pass_buf, strlen(pass_buf));
    }

    ascii_increment(pass_buf);
    strncpy(buf, pass_buf, buf_size);
}


// Do the work on the data
void
gpw_run(void* run_data)
{
    if (run_data == NULL) {
        goto DONE;
    }

    {
        FILE *fp;

        fp = fopen("./hackme","r");
        if (fp == NULL) {
            printf ("fopen() failed: %s\n", strerror(errno));
            exit(1);
        }
      
        EVP_PKEY * key = PEM_read_PrivateKey(fp, NULL, NULL, run_data);
        if (key == NULL) {
            fclose(fp);
            // ERR_print_errors_fp(stderr);
            goto DONE;
        }

        fclose(fp);
        printf("success! > %s <\n", (char *)run_data);
        exit(1);
    }

DONE:
    return;
}
