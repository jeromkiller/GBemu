#name of the executable
FILENAME = gameBoy

#File Directories
#Root dir where sub dirs are located
RDIR=GameBoyEmu
#Header dir
HDIR=$(RDIR)/headers
#Source dir
SDIR=$(RDIR)/source
#object dir
ODIR=$(RDIR)/.obs

#compiler and flags to use
CC=gcc
CFLAGS= -Wall -I $(HDIR)

#File paths
SRCS := $(wildcard $(SDIR)/*.c)
HEAD := $(wildcard $(HDIR)/*.h)
OBJS := $(addprefix $(ODIR)/, $(patsubst %.c, %.o, $(notdir $(SRCS))))

#build .o files
$(ODIR)/%.o : $(SDIR)/%.c $(HEAD)
	$(CC) -o $@ -c $< $(CFLAGS)

#build project 
$(FILENAME) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

#before objects can be built, the object folder has to be created
$(OBJS): | $(ODIR)

$(ODIR):
	mkdir $(ODIR)

#make clean
.PHONY : clean
clean :
	rm -r $(ODIR)
	rm $(FILENAME)