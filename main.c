#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int main(void)
{
	int sock;

	// create a socket
	// protocol family: can // socket type: raw // socket protocol: can raw
	if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
	{
		perror("Socket creation failed");
		return 1;
	}

	struct ifreq ifr;

	// retrieve the index interface based on interface name
	// use interface index 0 for all can interfaces
	strcpy(ifr.ifr_name, "can0");
	ioctl(sock, SIOCGIFINDEX, &ifr);

	struct sockaddr_can addr;
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	// bind socket to can interface
	if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("Socket binding failed");
		return 1;
	}

	struct can_frame frame; // create can frame

	frame.can_id = 0x1234; // can id
	frame.can_dlc = 5; // payload size
	sprintf(frame.data, "hello"); // write data to can frame

	// writes n bytes over can socket, returns number of bytes written
	if (write(sock, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame))
	{
		perror("Write failed");
		return 1;
	}

	// close socket
	if (close(sock) < 0)
	{
		perror("Closing socket failed");
		return 1;
	}

	return 0;
}