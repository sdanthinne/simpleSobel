TARGET   = runMe
CC       = g++
CCFLAGS  =  -Wall -O0 
LDFLAGS  = -lm `pkg-config --cflags --libs opencv`
SOURCES  = $(wildcard *.cpp)
INCLUDES = $(wildcard *.hpp)
OBJECTS  = $(SOURCES:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJECTS)
		$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(INCLUDES)

$(OBJECTS):$(SOURCES) $(INCLUDES)
		$(CC) -c $(CCFLAGS) $(SOURCES)

.PHONY: clean
clean:
		rm -f $(TARGET) $(OBJECTS)
