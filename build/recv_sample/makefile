CC        = gcc
CFLAGS   = -Wall -O2
LDFLAGS  = -lrt -pthread
LIBS     =
INCLUDE  =
BINDIR   = ../../bin
PROGRAM  = $(BINDIR)/recv_sample
SRCDIR   = ../../src
OBJDIR   = ../../obj
SMPLDIR  = recv_sample
QUEUEDIR = queue
SRCES    = $(SRCDIR)/$(SMPLDIR)/main.c $(SRCDIR)/$(QUEUEDIR)/queue.c
OBJS_TMP = $(SRCES:%.c=%.o)
OBJS     = $(OBJS_TMP:$(SRCDIR)%=$(OBJDIR)%)

$(PROGRAM): $(OBJS) $(LIBS)
	@if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR); fi
	@if [ ! -d $(OBJDIR)/$(QUEUEDIR) ]; then mkdir -p $(OBJDIR)/$(QUEUEDIR); fi
	@if [ ! -d $(OBJDIR)/$(SMPLDIR) ];  then mkdir -p $(OBJDIR)/$(SMPLDIR); fi
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(PROGRAM)

clean:
	rm -f $(OBJS) $(PROGRAM)
