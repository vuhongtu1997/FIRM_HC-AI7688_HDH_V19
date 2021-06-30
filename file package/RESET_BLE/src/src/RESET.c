/* standard headers */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

// transmit
static char getpro[]                  =   {0xe9, 0xff, 0x0c};
static char reset_gateway[]           =   {0xe9, 0xff, 0x02};
static char setpro_internal[]       =   {0xe9, 0xff, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44, 0x01, 0x00};
static char admit_pro_internal[]    =   {0xe9, 0xff, 0x0d, 0x01, 0x00, 0xff, 0xfb, 0xeb, 0xbf, 0xea, 0x06, 0x09, 0x00, 0x52, 0x90, 0x49, 0xf1, 0xf1, 0xbb, 0xe9, 0xeb};// trả về unicast tiếp theo của con đèn cần thêm vào

int serial_port;

int UART_Init()
{
	serial_port = open("/dev/ttyS1", O_RDWR);
	struct termios tty;
	// Read in existing settings, and handle any error
	if(tcgetattr(serial_port, &tty) != 0) {
	  printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	  return 1;
	}

	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	// tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
	// tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

	tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;

	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		return 1;
	}
}

void RESET_GW()
{
	write(serial_port, reset_gateway, sizeof(reset_gateway));
}

void GETPRO()
{
	write(serial_port, getpro, sizeof(getpro));
}

void SETPRO_INTERNAL()
{
	srand((int)time(0));
	int random;
	int i;
	for(i=0;i<16;i++)
	{
		random=rand()%256;
		setpro_internal[i+3]=random;
	}

	for(i=0;i<28;i++)
	{
		printf ("%x ",setpro_internal[i]);
	}
	puts("setpro...");
	write(serial_port, setpro_internal, sizeof(setpro_internal));
}

void ADMITPRO_INTERNAL()
{
	int i;
	for(i=0;i<21;i++)
	{
		printf ("%x ",admit_pro_internal[i]);
	}
	puts("admitpro...");
	write(serial_port, admit_pro_internal, sizeof(admit_pro_internal));
}

int main(int argc, char** argv)
{
	UART_Init();
	RESET_GW();
	sleep(6);
	puts("DONE!");
}
