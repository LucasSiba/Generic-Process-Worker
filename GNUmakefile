CPPFLAGS  = -I/usr/include/openssl/ # include paths, '.' is implicit
CFLAGS    = -O9 -g -rdynamic -fstack-protector -fno-strict-aliasing -Wall -Werror -Wextra -Wcast-align -Wcast-qual -Wformat=2 -Wformat-security -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wno-unknown-pragmas -Wunused -Wno-unused-result -Wwrite-strings -Wno-attributes
LDFLAGS   = # linker options (like -L for library paths)
LDLIBS    = -lm -lcrypto -lrt # libraries to link with

all: generic-process-worker

generic-process-worker: generic-process-worker.o example-functions.o

.PHONY: clean

clean:
	rm -f generic-process-worker *.o *.a *.t *.pass *.fail *.gcov *.gcda *.gcno

.PRECIOUS: %.t

# n.o: n.c
# 	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
# n: n.o
#	$(CC) $(LDFLAGS) n.o $(LDLIBS)

# $@ - The file name of the target of the rule
# $< - The name of the first prerequisite
# $^ - The names of all the prerequisites, with spaces between them.
# $* - The stem with which an implicit rule matches

