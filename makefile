#name of the executable
FILENAME = gameBoy
TESTPROGRAM = gameBoyTest

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
CFLAGS:= -Wall -g -I $(HDIR)

#File paths
SRCS := $(wildcard $(SDIR)/*.c)
HEAD := $(wildcard $(HDIR)/*.h)
OBJS := $(addprefix $(ODIR)/, $(patsubst %.c, %.o, $(notdir $(SRCS))))

#excludes
MAINEXCLUDES:=$(ODIR)/CPU_Test.o
TESTEXCLUDES:=$(ODIR)/GameBoyEmu.o

#build .o files
$(ODIR)/%.o : $(SDIR)/%.c $(HEAD) 
	@echo building object: $@;
	@$(CC) -o $@ -c $< $(CFLAGS)

#build project 
$(FILENAME) : $(filter-out $(MAINEXCLUDES), $(OBJS))
	@echo building executable: $@;
	@$(CC) -o $@ $^ $(CFLAGS)

#before objects can be built, the object folder has to be created
$(OBJS): | $(ODIR)

$(ODIR):
	@mkdir $(ODIR)

#make clean
.PHONY : clean
clean :
	@echo cleaning files
	@rm -r $(ODIR)
	@rm $(FILENAME)

#build the version of the program
$(TESTPROGRAM) : $(filter-out $(TESTEXCLUDES), $(OBJS))
	@echo building executable: $@;
	@$(CC) -o $@ $^ $(CFLAGS)

#make tests
#build and run the tests automaticaly
#cleans itself up afterwards
.PHONY : test
test : $(TESTPROGRAM)
	@python3 ./Testing/TestScript.py
	@rm $(TESTPROGRAM)
