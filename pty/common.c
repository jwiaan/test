#include "common.h"

static int io(int i, int o)
{
	char b[4096];
	ssize_t r = read(i, b, sizeof(b));
	if (r < 0) {
		perror("read");
		return -1;
	}

	if (r == 0) {
		printf("%d eof\n", i);
		return -1;
	}

	ssize_t w = write(o, b, r);
	if (w < 0) {
		perror("write");
		return -1;
	}

	assert(w == r);
	return 0;
}

void loop(int x, int y)
{
	struct pollfd p[2] = { {x, POLLIN}, {y, POLLIN} };
	while (poll(p, 2, -1) > 0) {
		for (int i = 0; i < 2; i++) {
			if (p[i].revents & POLLIN) {
				if (io(p[i].fd, p[(i + 1) % 2].fd))
					return;
			}
		}
	}

	perror("poll");
}

int start(const char *host, const char *port, const struct addrinfo *hint,
	  int (*action)(int, const struct sockaddr *, socklen_t))
{
	struct addrinfo *a, *l;
	int e = getaddrinfo(host, port, hint, &l);
	if (e) {
		fprintf(stderr, "%s\n", gai_strerror(e));
		exit(1);
	}

	for (a = l; a; a = a->ai_next) {
		int s = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
		if (s < 0) {
			perror("socket");
			continue;
		}

		int i = 1;
		setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
		if (!action(s, a->ai_addr, a->ai_addrlen)) {
			freeaddrinfo(l);
			return s;
		}

		perror("action");
		close(s);
	}

	exit(1);
}
