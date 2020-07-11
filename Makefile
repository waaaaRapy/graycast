TARGET = build/graycast
SRCS   = $(wildcard src/*.c)
HEADS  = $(wildcard src/*.h)
OBJS   = $(SRCS:src/%.c=build/%.o)
CFLAGS = -std=c11 -g -static


$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

build/%.o: src/%.c $(HEADS)
	@[ -d build ] || (echo "mkdir build" && mkdir build)
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(TARGET)
	cd build && ../test/test.sh

clean:
	rm -rf build

.PHONY: test clean
