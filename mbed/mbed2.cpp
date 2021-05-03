#include "mbed.h"
#include "LSM9DS1.h"
#include "rtos.h"
#include "motordriver.h"
#include "XNucleo53L0A1.h"
#include <stdio.h>
 
Serial pc(USBTX, USBRX);
Serial blue(p13,p14);
LSM9DS1 imu(p9, p10, 0xD6, 0x3C);

DigitalOut myled(LED1);
DigitalOut shdn(p26);

Thread thread1;
Thread thread2;
Thread thread3;

volatile bool   obs = false;
volatile bool   forward = true;
volatile bool   finished = false;

#define RANGE(x) ((x) > 0 && (x) < 100)

//I2C sensor pins
#define VL53L0_I2C_SDA   p28
#define VL53L0_I2C_SCL   p27

//See http://mbed.org/cookbook/Motor
//Connections to dual H-brdige driver for the two drive motors
Motor  left(p21, p29, p30, 1); // pwm, fwd, rev, has brake feature
Motor right(p22, p23, p24, 1);

static XNucleo53L0A1 *board=NULL;

#define REFRESH 2000

float oldX = 0;
float oldY = 0;
float oldZ = 0;
float x= 0;
float y = 0;
float z  = 0;

float posx = 0;
float velx = 0;
float oldPosx = 0;
float oldVelx = 0;

float posy = 0;
float vely = 0;
float oldPosy = 0;
float oldVely = 0;

float posz = 0;
float velz = 0;
float oldPosz = 0;
float oldVelz = 0;

float moveCount = 1;
float turnCount = 1;

int status;
uint32_t distance;

int row;
int column;

void init_lidar() {
//    int status;
    DevI2C *device_i2c = new DevI2C(VL53L0_I2C_SDA, VL53L0_I2C_SCL);
    /* creates the 53L0A1 expansion board singleton obj */
    board = XNucleo53L0A1::instance(device_i2c, A2, D8, D2);
    shdn = 0; //must reset sensor for an mbed reset to work
    Thread::wait(100);
    shdn = 1;
    Thread::wait(100);
    /* init the 53L0A1 board with default values */
    status = board->init_board();
    while (status) {
        pc.printf("Failed to init board! \r\n");
        status = board->init_board();
    }
}

// object detected - takes robot around and returns to column
void object_turn() {
    if (forward) {
        left.speed(0.5);
        right.speed(-0.1);
        Thread::wait(700);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        left.speed(0.25);
        right.speed(0.25);
        Thread::wait(1500);
        left.speed(0.0);
        right.speed(0.0);
        
        Thread::wait(200);
        left.speed(-0.5);
        right.speed(0.1);
        Thread::wait(700);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        left.speed(0.25);
        right.speed(0.25);
        Thread::wait(1500);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        
        left.speed(-0.5);
        right.speed(0.1);
        Thread::wait(700);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        left.speed(0.25);
        right.speed(0.25);
        Thread::wait(1500);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        
        left.speed(0.1);
        right.speed(-0.5);
        Thread::wait(700);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        left.speed(0.25);
        right.speed(0.25);
        Thread::wait(1500);
        left.speed(0.0);
        right.speed(0.0);
        
    } else {
        
        right.speed(0.5);
        left.speed(-0.1);
        Thread::wait(700);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        left.speed(0.25);
        right.speed(0.25);
        Thread::wait(1500);
        left.speed(0.0);
        right.speed(0.0);
        
        Thread::wait(200);
        right.speed(-0.5);
        left.speed(0.1);
        Thread::wait(700);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        left.speed(0.25);
        right.speed(0.25);
        Thread::wait(1500);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        
        right.speed(-0.5);
        left.speed(0.1);
        Thread::wait(700);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        right.speed(0.25);
        left.speed(0.25);
        Thread::wait(1500);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        
        right.speed(0.1);
        left.speed(-0.5);
        Thread::wait(700);
        left.speed(0.0);
        right.speed(0.0);
        Thread::wait(200);
        left.speed(0.25);
        right.speed(0.25);
        Thread::wait(1500);
        left.speed(0.0);
        right.speed(0.0);
    }    
}

void lidar_thread() {
//    pc.printf("in lidar thread");
    while(1) {
//        pc.printf("in lidar loop");
        status = board->sensor_centre->get_distance(&distance);
//        pc.printf("got lidar status");
        if (status == VL53L0X_ERROR_NONE) {
//            pc.printf("in lidar if #1");
            pc.printf("D=%ld mm\r\n", distance);
            pc.printf("Checking obstacles...\n");
            obs = RANGE(distance);
            if (obs == true) {
//                pc.printf("in lidar obs if");
                pc.printf("Obstacle detected straight ahead\n");
                pc.printf("D=%ld mm\r\n", distance);
                pc.printf("Stopping\n");
                left.speed(0.0);
                right.speed(0.0);
                object_turn();
                Thread::wait(200);
            } else {
                pc.printf("No obstacles detected... moving forward\n");
                left.speed(0.25);
                right.speed(0.25);
            }
            Thread::wait(500);
        }
        Thread::wait(100);
    
    }
}

void imu_thread() {
    while(1) {
        imu.readAccel();
        imu.readMag();
        imu.readGyro();
 
        pc.printf("gyro: %d %d %d\n\r", imu.gx, imu.gy, imu.gz);
        pc.printf("accel: %d %d %d\n\r", imu.ax, imu.ay, imu.az);
        pc.printf("mag: %d %d %d\n\n\r", imu.mx, imu.my, imu.mz);

        velx = (oldVelx + REFRESH * oldX)/1000;     
        posx = (oldPosx + REFRESH * oldVelx)/1000;
        
        vely = (oldVely + REFRESH * oldY)/1000;     
        posy = (oldPosy + REFRESH * oldVely)/1000;
        
        velz = (oldVelz + REFRESH * oldZ)/1000;     
        posz = (oldPosz + REFRESH * oldVelz)/1000;
        
        pc.printf("X Pos: %2f\n\r",posx);
        pc.printf("Y Pos:%2f\n\r",posy);
        pc.printf("Z Pos: %2f\n\r",posz);  
        
        oldVelx = velx;
        oldPosx = posx;
        oldVely = vely;
        oldPosy = posy;
        oldPosz = posz;
        oldVelz = velz;
        oldX = imu.ax;
        oldY = imu.ay;
        oldZ = imu.az;
        
    Thread::wait(2000);
    }
}

void turn_right() {
    left.speed(-0.5);
    right.speed(0.1);
    Thread::wait(1400);
    left.speed(0.0);
    right.speed(0.0);
}

void turn_left() {
    right.speed(-0.5);
    left.speed(0.1);
    Thread::wait(1400);
    left.speed(0.0);
    right.speed(0.0);
}

void driving_thread() {
    while(1) {
        if (forward) {
            if (posx >= 2000 && posy <= 2000) {
                left.speed(0.0);
                right.speed(0.0);
                turn_left();
                forward = !forward;
            }
        } else {
            if (posx <= 0 && y <= 1500) {
                left.speed(0.0);
                right.speed(0.0);
                turn_right();
                forward = !forward;
            }
        }

        if (moveCount < 3 && turnCount < 5) {
            left.speed(0.25);
            right.speed(0.25);
            Thread::wait(2000);
            left.speed(0.0);
            right.speed(0.0);
            Thread::wait(2000);
            moveCount++;
            row++;
        } else if (moveCount == 3 && turnCount < 5) {
            moveCount = 0;
            left.speed(0.0);
            right.speed(0.0);
            Thread::wait(1000);
            if (forward) {
                turn_left();
            } else {
                turn_right();
            }
            turnCount++;
            forward = !forward;
            column++;
        } else {
            left.speed(0.0);
            right.speed(0.0);
        }
        Thread::wait(100);
        pc.printf("Tile: (%i, %i)", row, column);
    }
}

int main() {
    //Thread::wait(5000);
    imu.begin();
    if (!imu.begin()) {
        pc.printf("Failed to communicate with LSM9DS1.\n");
    }
    imu.calibrate();
    init_lidar();
    thread1.start(lidar_thread);
    thread2.start(imu_thread);
    thread3.start(driving_thread);

    while(1) {
        myled = !myled;      
        Thread::wait(500);
    }
}
