TARGET   = runMe
CC       = g++
CCFLAGS  =  -Wall -O0 `pkg-config --cflags --libs opencv4`
LDFLAGS  = -lm `pkg-config --cflags --libs opencv4`
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
