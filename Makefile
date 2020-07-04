TARGET = build/graycast
SRCS   = $(wildcard src/*.c)
OBJS   = $(SRCS:src/%.c=build/%.o)
CFLAGS = -std=c11 -g -static


$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): $(SRCS)
	@[ -d build ] || (echo "mkdir build" && mkdir build)
	$(CC) -c -o $@ $<

test: $(TARGET)
	cd build && ../test/test.sh

clean:
	rm -rf build

.PHONY: test clean
