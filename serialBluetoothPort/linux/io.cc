#include <iostream>
#include <thread>
#include <fstream>
#include <string>

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */
#include <errno.h>   /* ERROR Number Definitions           */
#include <stdlib.h>

using namespace std;

void write_serial_port() {
  char rd_ch = '\0';
  int fd = open("/dev/rfcomm0", O_WRONLY);

  struct termios SerialPortSettings;	/* Create the structure                          */

  tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

  /* Setting the Baud rate */
  cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
  cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

  /* 8N1 Mode */
  SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
  SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
  SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
  SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

  SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
  SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


  SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
  SerialPortSettings.c_iflag &= ~(ICANON | ECHOE | ISIG);  /* Non Cannonical mode                            */

  SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

  /* Setting Time outs */
  SerialPortSettings.c_cc[VMIN] = 40; /* Read at least 10 characters */
  SerialPortSettings.c_cc[VTIME] = 10; /* Wait indefinetly   */


  if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
    printf("\n  ERROR ! in Setting attributes");
  else
    printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");


  while(true) {
    cin >> rd_ch;
    int write_count = write(fd, &rd_ch, 1);
  }

  close(fd); /* Close the serial port */
}

void read_serial_port() {
  char rd_ch = '\0';
  int fd = open("/dev/rfcomm0", O_RDONLY);

  struct termios SerialPortSettings;	/* Create the structure                          */

  tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

  /* Setting the Baud rate */
  cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
  cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

  /* 8N1 Mode */
  SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
  SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
  SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
  SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

  SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
  SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


  SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
  SerialPortSettings.c_iflag &= ~(ICANON | ECHOE | ISIG);  /* Non Cannonical mode                            */

  SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

  /* Setting Time outs */
  SerialPortSettings.c_cc[VMIN] = 40; /* Read at least 10 characters */
  SerialPortSettings.c_cc[VTIME] = 10; /* Wait indefinetly   */


  if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
    printf("\n  ERROR ! in Setting attributes");
  else
    printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");

  tcflush(fd, TCIFLUSH);
  // fstream log("./thread_log.txt", ios::out);
  while(true) {
    int read_count = read(fd, &rd_ch, 1);
    if(read_count > 0) {
      cout << rd_ch << flush;
    }
  }
  close(fd);
}

int main()
{
  thread t(read_serial_port);
  thread t1(write_serial_port);
  t1.join();
  t.join();
}
