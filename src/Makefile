TARGET   = runMe
CC       = g++-10.1
CCFLAGS  =  -Wall -O0 `pkg-config --cflags opencv` -lpthread -mfpu=neon -mcpu=Cortex-A8
LDFLAGS  = -lm `pkg-config  --libs --cflags opencv` -lpthread
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
