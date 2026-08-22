# Scheduler & Watchdog Library Makefile

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c11 -O2 -I./include
LDFLAGS =

# Source directories
SRCDIR = src
TESTDIR = tests
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Library sources
LIB_SRCS = $(wildcard $(SRCDIR)/*.c)
LIB_OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(LIB_SRCS))

# Test sources
TEST_SRCS = $(wildcard $(TESTDIR)/*.c)
TEST_BINS = $(patsubst $(TESTDIR)/%.c,$(BINDIR)/%,$(TEST_SRCS))

# Default target
all: dirs $(OBJDIR)/libds_sched_wd.a

# Create directories
dirs:
	@mkdir -p $(OBJDIR) $(BINDIR)

# Build library object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build static library
$(OBJDIR)/libds_sched_wd.a: $(LIB_OBJS)
	ar rcs $@ $^

# Build test executables
$(BINDIR)/%: $(TESTDIR)/%.c $(OBJDIR)/libds_sched_wd.a
	$(CC) $(CFLAGS) $< -L$(OBJDIR) -lds_sched_wd $(LDFLAGS) -o $@

# Run all tests
test: all $(TEST_BINS)
	@echo "Running tests..."
	@for test in $(TEST_BINS); do echo "=== $$test ==="; ./$$test || exit 1; done
	@echo "All tests passed."

# Clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Phony targets
.PHONY: all dirs test clean
