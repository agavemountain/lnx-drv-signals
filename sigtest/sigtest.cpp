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
#include <unistd.h>

struct sigaction act;

void signalReceived(int sig)
{
	printf("Received signal - %d\n", sig);

}

int main(int argc,char **argv)
{
	act.sa_flags=0;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGINT);
	act.sa_handler = signalReceived;

	sigaction(SIGINT, &act, 0);

	while (1) {
		printf("hello.\n");
		sleep(1);
	}
	return 0;
}

