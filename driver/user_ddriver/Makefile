CC        = gcc 
CFLAGS    = -Wall -O -g 
CXXFLAGS  =
TARGET    = libddriver.a
LIBPATH   = ${HOME}/lib/

OBJS      = ddriver.o
SRCS      = ddriver.c

$(OBJS):$(SRCS)
	$(CC) $(CFLAGS) -c $^

all:$(OBJS)
	ar rcs $(TARGET) $^
	mkdir -p $(LIBPATH)
	mv -f $(TARGET) $(LIBPATH)

clean:
	rm -f *.o
	rm -f $(LIBPATH)$(TARGET)