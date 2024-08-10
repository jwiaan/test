#pragma once

#include <assert.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

void loop(int, int);
int start(const char *, const char *, const struct addrinfo *,
	  int (*)(int, const struct sockaddr *, socklen_t));
