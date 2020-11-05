TARGET   = runMe
CC       = g++
CCFLAGS  =  -Wall -O0 `pkg-config --cflags opencv` -lpthread -mfpu=neon -mcpu=Cortex-A8 -ffast-math -ggdb
LDFLAGS  = -lm `pkg-config  --libs --cflags opencv` -lpthread -lpfm -ggdb

SOURCES  = $(wildcard *.cpp)
INCLUDES = $(wildcard *.hpp)
OBJDIR   = ../obj/
OBJECTS  = $(addprefix $(OBJDIR),$(SOURCES:.cpp=.o))

all:$(TARGET)

$(TARGET):$(OBJECTS)
		$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(INCLUDES)

$(OBJECTS):$(SOURCES) $(INCLUDES)
		$(CC) -c $(CCFLAGS) $(SOURCES)
		`mkdir -p ../obj`
		`mv *.o ../obj/.`

.PHONY: clean
clean:
		rm -f $(OBJECTS)
		rm -f $(SOURCES:.cpp=.o)

.PHONY: debug
debug: 
		gdb --args ./$(TARGET) ../res/test3.avi

.PHONY: perf
perf:
		$(CC) -c $(CCFLAGS) -pg $(SOURCES)
		$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS) -pg $(INCLUDES)
		./$(TARGET) $(args)
		gprof $(TARGET) > $(TARGET).output
.PHONY: assm
assm:
	$(CC) -S -fverbose-asm -fomit-frame-pointer -g $(CCFLAGS) $(SOURCES)
		
