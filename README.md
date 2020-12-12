# Threaded Sobel Edge Detection
This codebase represents a sobel kernel multiplicatoin tool, with many optimizations to run on the raspberry pi 3 B+.
These optimizations include:
-multi-threading
-ARM NEON
-memory use reduction using hardware counters
-compiler optimization

### Dependencies:
-make
-gcc
-pkg-config
-opencv4
-pthread compatability

### Building:
`make` to build on your own system 
executable will be named runMe

### Running Sobel
> ./runMe
> Two options
> filename.avi
> -v Run using the video at port 0
