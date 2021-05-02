#include "mbed.h"
#include "StepperMotor_X27168.h"
#include "XNucleo53L0A1.h"
#include "Motor.h"

#define MOTOR_SPEED 0.28
#define READ_BUF_SIZE 150

DigitalOut myled(LED1);
InterruptIn left_edge(p26);
InterruptIn right_edge(p25);
Motor motor(p24,p23,p22);

Timer tmr;
bool has_data = false;
bool collect_data = false;
bool should_send_buffer = false;
bool wait_for_signal = true;
int read_buffer[READ_BUF_SIZE];
int read_buffer_next_idx = 0;

Serial pc(p13,p14);
RawSerial pi(USBTX, USBRX);
DigitalOut shdn(p29);
// This VL53L0X board test application performs a range measurement in polling mode
// Use 3.3(Vout) for Vin, p28 for SDA, p27 for SCL, P29 for shdn on mbed LPC1768

//I2C sensor pins
#define VL53L0_I2C_SDA   p28
#define VL53L0_I2C_SCL   p27

static XNucleo53L0A1 *board=NULL;

void init_lidar() {
    int status;
    DevI2C *device_i2c = new DevI2C(VL53L0_I2C_SDA, VL53L0_I2C_SCL);
    /* creates the 53L0A1 expansion board singleton obj */
    board = XNucleo53L0A1::instance(device_i2c, A2, D8, D2);
    shdn = 0; //must reset sensor for an mbed reset to work
    wait(0.1);
    shdn = 1;
    wait(0.1);
    /* init the 53L0A1 board with default values */
    status = board->init_board();
    while (status) {
        pc.printf("Failed to init board! \r\n");
        status = board->init_board();
    }
}

void send_read_buffer() {
    char buf[4*READ_BUF_SIZE], *pos=buf;
    for (int i = 0 ; i < read_buffer_next_idx ; i++) {
        if (i) {
            *pos = ' ';
            pos++;
        }
        pos += sprintf(pos, "%d", read_buffer[i]);
    }
    pi.puts(buf);
    pi.putc('\0');
}

void left_edge_contact() {
    //TODO: send reading buffer
    collect_data = false;
    motor.speed(MOTOR_SPEED);
    if (has_data && !should_send_buffer) {
        read_buffer[read_buffer_next_idx] = tmr.read_ms();
        read_buffer_next_idx++;
        send_read_buffer();
        should_send_buffer = false;
    }
    
}

void right_edge_contact() {
    motor.speed(0);
    wait_for_signal = true;
}


int main() {
    int status;
    uint32_t distance;
    
    left_edge.mode(PullUp);
    right_edge.mode(PullUp);
    
    left_edge.fall(left_edge_contact);
    right_edge.fall(right_edge_contact);
    
    init_lidar();
    motor.speed(0.2);
    tmr.start();
    
    while(1) {
        if (collect_data) {
            do {
                status = board->sensor_centre->get_distance(&distance);
            } while (status != VL53L0X_ERROR_NONE);
            if (distance > 0) {
                read_buffer[read_buffer_next_idx] = tmr.read_ms();
                read_buffer[read_buffer_next_idx + 1] = (int)distance;
                read_buffer_next_idx += 2;
                has_data = true;
            }
        }
        if (wait_for_signal) {
            if (pi.getc() == '1') {
                motor.speed(-MOTOR_SPEED);
                collect_data = true; 
                has_data = false;
                read_buffer_next_idx = 0;
                tmr.reset(); 
                wait_for_signal = false;
            }
        }
        myled = 1-myled;
    }
            
}

