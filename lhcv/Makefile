CC = cc
CFLAGS = -std=gnu11 -Wall -Wextra -Werror -pedantic -pthread
LDLIBS = -pthread

.PHONY: all clean
all: sync_car

sync_car: sync_car.o sync_car-runner.o

sync_car.o: sync_car.c sync_car.h

# O runner original possui um parametro nao utilizado no tratador de sinal.
sync_car-runner.o: CFLAGS += -Wno-unused-parameter
sync_car-runner.o: sync_car-runner.c sync_car.h

clean:
	rm -f sync_car sync_car.o sync_car-runner.o
