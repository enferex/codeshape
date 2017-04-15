APP    = codeshape
CFLAGS = -g3 -O0
CC     = gcc

all: $(APP)

$(APP): main.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	$(RM) $(APP)
