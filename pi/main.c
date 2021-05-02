#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <math.h>

#define PI 3.14159265
#define ARC_START PI/4
#define ARC_LENGTH PI/2

struct Point {
 float x;
 float y;
};

int n_lidar_points = 0;
struct Point lidar_points[50];

void parse_lidar(char* data, int length) {
	char* start_ptr = data;
	
	// parse the string to an int[]
	int i = 0;
	int my_data[100];
	int n_data = 0;
	while (i < length) {
		while (i < length && data[i] != ' ') {
			i++;
		}
		if (i < length) {
			data[i] = '\0';
		}
		my_data[n_data] = atoi(start_ptr);
		i++;
		n_data++;
		start_ptr = data + i;
	}
	
	// use trig to convert distances and times to points
	float max_time = (float)my_data[n_data-1];
	float angle, dist;
	int curr_lidar_idx = 0;
	struct Point tmp;
	for (i = 0; i < n_data-1; i+=2) {
		angle = ARC_START + (my_data[i]/max_time)*ARC_LENGTH;
		dist = my_data[i+1]/10.0; // mm to cm
		tmp.x = dist*cos(angle);
		tmp.y = dist*sin(angle);
		lidar_points[curr_lidar_idx] = tmp;
		curr_lidar_idx++;
	}
	n_lidar_points = curr_lidar_idx;
}
 
int main(int argc, char ** argv) {
  int fd;
  char buf[256];
  // Open the Port. We want read/write, no "controlling tty" status, 
  // and open it no matter what state DCD is in
  fd = open("/dev/ttyACM1", O_RDWR | O_NOCTTY | O_NDELAY);  
  //open mbed's USB virtual com port
  if (fd == -1) {  
    perror("open_port: Unable to open /dev/ttyACM0 - ");
    return(-1);
  }
  // Turn off blocking for reads, use (fd, F_SETFL, FNDELAY) if you want that
  fcntl(fd, F_SETFL, 0);
  //Linux Raw serial setup options
  struct termios options; 
  tcgetattr(fd,&options);   //Get current serial settings in structure
  cfsetspeed(&options, B9600);   //Change a only few
  options.c_cflag &= ~CSTOPB;
  options.c_cflag |= CLOCAL;
  options.c_cflag |= CREAD;
  cfmakeraw(&options);
  tcsetattr(fd,TCSANOW,&options);    //Set serial to new settings
  sleep(1);
  
  char message;
  
  char should_read;
  int bytes_read, next_buf_idx;
  write(fd,"1",1);
  while (1) {
	should_read = 1;
	next_buf_idx = 0;
	while (should_read) {
	  bytes_read = read(fd, &message, 1);
	  if (bytes_read) {
		buf[next_buf_idx] = message;
		next_buf_idx++;
	  }
	  if (message == '\0') {
		should_read = 0;
	  }
	}
	printf("%s\n", buf);
	parse_lidar(buf, next_buf_idx);
	for (int i = 0; i < n_lidar_points; i++) {
		printf("x: %0.2f, y: %0.2f\n", lidar_points[i].x, lidar_points[i].y);
	}
	
  }
  
  
  
  // Don't forget to clean up and close the port
  tcdrain(fd);
  close(fd);
  return 0;
}
