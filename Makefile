CC = gcc
CFLAGS = -std=gnu99 -Wall -g -pthread
CHECK_CFLAGS = -std=gnu99 -Wall -g -pthread -DTEST
OBJS = list.o threadpool.o main.o

.PHONY: all clean test

GIT_HOOKS := .git/hooks/pre-commit

all: $(GIT_HOOKS) sort

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

deps := $(OBJS:%.o=.%.o.d)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF .$@.d -c $<

sort: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -rdynamic

test: sort
	for i in `seq 1 1 1000`; do \
		uniq words.txt | sort -R > input.txt		
		for j in 1 2 4 8 16; do \
			./sort $$j 349900; \
		done > output.txt; \
	done;

check_right :
	$(CC) $(CHECK_CFLAGS) -o list.o -MMD -MF .list.o.d -c list.c
	$(CC) $(CHECK_CFLAGS) -o threadpool.o -MMD -MF .threadpool.o.d -c threadpool.c
	$(CC) $(CHECK_CFLAGS) -o main.o -MMD -MF .main.o.d -c main.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) -rdynamic
	uniq words.txt | sort -R > input.txt
	./check_right 4 349900 < input.txt > output.txt
	diff words.txt output.txt

generate:
	gcc -o genran genran.c

clean:
	rm -f $(OBJS) sort check_right
	@rm -rf $(deps)

-include $(deps)

