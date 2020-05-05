CC=gcc
# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

TARGET = testAudioFFT

all : $(TARGET)

$(TARGET): timeSpaceToFreqDom.c
	$(CC) $(CFLAGS) -o $(TARGET) /usr/lib64/libsndfile.so.1.0.28 timeSpaceToFreqDom.c -lfftw3 -lm

clean:
	$(RM) $(TARGET)
	$(RM) timeDomain
	$(RM) freqDomain
