COMPILER = gcc
CFLAGS   = -Wall -O2
LDFLAGS  = -lrt -pthread
LIBS     =
INCLUDE  =
PROGRAM  = ../../bin/recv_sample
SRCDIR   = ../../src
OBJDIR   = ../../obj
SRCES    = $(SRCDIR)/recv_sample/main.c $(SRCDIR)/queue/queue.c
OBJS_TMP = $(SRCES:%.c=%.o)
OBJS     = $(OBJS_TMP:$(SRCDIR)%=$(OBJDIR)%)

$(PROGRAM): $(OBJS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(PROGRAM)

clean:
	rm -f $(OBJS) $(PROGRAM)
