CC = gcc
CFLAGS = -std=gnu99 -Wall -g -pthread
CHECK_CFLAGS = -std=gnu99 -Wall -g -pthread
OBJS = list.o threadpool.o main.o

.PHONY: all clean test

GIT_HOOKS := .git/hooks/pre-commit

all: $(GIT_HOOKS) sort

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

deps := $(OBJS:%.o=.%.o.d)
%.o: %.c
	$(CC) $(CHECK_CFLAGS) -o $@ -MMD -MF .$@.d -c $<

sort: $(OBJS)
	$(CC) $(CHECK_CFLAGS) -o $@ $(OBJS) -rdynamic

test: sort
	for i in `seq 1 1 100`; do \
		printf "%d" $$i;\
		uniq input.txt | sort -R > input1.txt; \
		for j in 1 2 4 8 16; do \
			./sort $$j 2000 < input1.txt; \
		done; \
		printf "\n"; \
	done > output.txt

generate:
	gcc -o genran genran.c

clean:
	rm -f $(OBJS) sort check_right
	@rm -rf $(deps)

-include $(deps)

