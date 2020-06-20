OBJS    = main.o comm/comm.o sensors/mpu6050.o i2c/I2Cdev.o
SOURCE  = main.c comm/comm.c sensors/mpu6050.c i2c/I2Cdev.c
HEADER  = comm/comm.h sensors/mpu6050.h sensors/mpu6050_registers.h i2c/I2Cdev.h
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

