
CC = gcc

USR_DIR = /usr/local/lib
PLUGIN_NAME = test.so
SONAME = $(PLUGIN_NAME)

TARGET = $(SONAME)

INCLUDEDIR = -I/usr/local/include

DEVC_FLAGS = -DLINUX
CFLAGS = -Wall -Wextra -O0 -fPIC -ggdb -g3 -fvisibility=hidden -fgnu89-inline \
	 $(INCLUDEDIR) -D_GNU_SOURCE

LIBDIR = -L/usr/local/lib
LIBS = -lcollections -lxante

OBJS = main.o

$(TARGET): $(OBJS)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $(TARGET) $(OBJS) $(LIBDIR) $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET) *~

purge: clean $(TARGET)

