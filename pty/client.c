#include "common.h"

int main(int, char *v[])
{
	struct addrinfo a = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_STREAM
	};

	int c = start(v[1], "1116", &a, connect);
	loop(c, 0);
}
