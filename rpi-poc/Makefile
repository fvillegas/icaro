OBJS    = main.o MahonyAHRS.o comm/comm.o sensors/mpu6050.o sensors/hcm5883l.o i2c/I2Cdev.o
SOURCE  = main.c MahonyAHRS.cpp comm/comm.c sensors/mpu6050.c sensors/hcm5883l.c i2c/I2Cdev.c
HEADER  = MahonyAHRS.h comm/comm.h sensors/mpu6050.h sensors/mpu6050_registers.h sensors/hcm5883l.h sensors/hcm5883l_registers.h i2c/I2Cdev.h
OUT     = main
CC       = g++
FLAGS    = -g -c -Wall
LFLAGS   = -lm

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c

clean:
	rm -f $(OBJS) $(OUT)

