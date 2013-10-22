/*
    sigtest.cpp

    Copyright (C) 2013 Joe Turner <joe@agavemountain.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
//#include <linux/ioctl.h>

#define SIGDRV_IOC_MAGIC	100
#define IOCTL_PING  _IO(SIGDRV_IOC_MAGIC, 0)
// #define IOCTL_PING 9999
#include <sys/ioctl.h>

int gotdata = 0;



void sighandler(int signo) {
	if (signo == SIGIO) {
		printf("Signal received!\n");
		gotdata++;
	}
	return;
}

char buffer[10];

int main(int argc,char **argv)
{
	int count;
	int status;
	struct sigaction act;
	const char *driver = "/dev/sigdrv";
	int fd;

	memset(&act, 0, sizeof(act));
	act.sa_handler = sighandler;
	act.sa_flags=0;

	sigaction(SIGIO, &act, 0);

	fd = open(driver, O_RDONLY);
	if (fd < 0 ) {
		printf("Could not open driver.\n");
		exit(1);
	}
	printf("fd : %d\n", fd);

	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | FASYNC);

	printf("sending ioctl %d\n", IOCTL_PING);

	while (1) {
		if (ioctl(fd, IOCTL_PING) == -1) {
			printf("IOCTL failed.\n");
			exit(1);
		}
		sleep (1);
		if (!gotdata) {
			continue;
		}
		// handle i/o request.
		gotdata=0;
	}


//
//
//	while (1) {
//		printf("hello.\n");
//		sleep(1);
//	}
	return 0;
}

