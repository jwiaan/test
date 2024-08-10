#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void loop(int c, int m)
{
	struct pollfd p[2] = { {c, POLLIN}, {m, POLLIN} };
	while (poll(p, 2, -1) > 0) {
		for (int i = 0; i < 2; i++) {
			if (p[i].revents & POLLIN) {
				char b[4096];
				ssize_t n = read(p[i].fd, b, sizeof(b));
				write(p[(i + 1) % 2].fd, b, n);
			}
		}
	}
}

int start(const char *port)
{
	struct addrinfo *a, *l, hint = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM
	};

	getaddrinfo(NULL, port, &hint, &l);
	for (a = l; a; a = a->ai_next) {
		int f = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
		if (f < 0) {
			perror("socket");
			continue;
		}

		if (!connect(f, a->ai_addr, a->ai_addrlen))
			return f;

		perror("connect");
		close(f);
	}

	exit(1);
}

int main(void)
{
	int c = start("1116");
	loop(0, c);
}
