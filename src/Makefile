TARGET   = runMe
CC       = g++
CCFLAGS  =  -Wall -O0 -lopencv_core 
LDFLAGS  = -lm -I /usr/include/opencv4 -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio
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
