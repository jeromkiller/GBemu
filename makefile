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
GTK_INCLUDE = -pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/fribidi -I/usr/include/harfbuzz -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/uuid -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
GTK_LIBS = -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0


#compiler and flags to use
CC=gcc
CFLAGS:= -Wall -g -I $(HDIR) $(GTK_INCLUDE) $(GTK_LIBS)

#File paths
SRCS := $(wildcard $(SDIR)/*.c)
HEAD := $(wildcard $(HDIR)/*.h)
OBJS := $(addprefix $(ODIR)/, $(patsubst %.c, %.o, $(notdir $(SRCS))))

#excludes
MAINEXCLUDES:=$(ODIR)/CPU_Test.o
TESTEXCLUDES:=$(ODIR)/GameBoyEmu.o $(ODIR)/main.o

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
