APP    = codeshape
CFLAGS = -g3 -O0
CC     = gcc

all: $(APP)

$(APP): main.c
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: test
test: $(APP)
	./$(APP) main.c > test.ppm && convert test.ppm test.png

clean:
	$(RM) $(APP)
