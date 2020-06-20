OBJS    = main.o comm.o sensors/mpu6050.o i2c/i2c.o
SOURCE  = main.c comm.c sensors/mpu6050.c i2c/i2c.c
HEADER  = comm.h sensors/mpu6050.h sensors/mpu6050_registers.h i2c/i2c.h
OUT     = main
CC       = gcc
FLAGS    = -g -c -Wall
LFLAGS   =

all: $(OBJS)
        $(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
        $(CC) $(FLAGS) main.c

clean:
        rm -f $(OBJS) $(OUT)
