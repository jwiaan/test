#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

void Close(int *q)
{
	int i = close(q[0]);
	assert(i == 0);
	i = close(q[1]);
	assert(i == 0);
}

void cat(int *q)
{
	int fd = dup2(q[1], STDOUT_FILENO);
	assert(fd == STDOUT_FILENO);
	Close(q);
	execlp("cat", "cat", "/etc/passwd", NULL);
	assert(0);
}

void grep(int *q)
{
	int fd = dup2(q[0], STDIN_FILENO);
	assert(fd == STDIN_FILENO);
	Close(q);
	execlp("grep", "grep", "root", NULL);
	assert(0);
}

int main(void)
{
	int q[2];
	pipe(q);

	void (*f[])(int *) = { cat, grep };
	for (int i = 0; i < sizeof(f) / sizeof(f[0]); ++i) {
		if (fork() == 0)
			f[i] (q);
	}

	Close(q);
	while (wait(NULL) != -1) ;
	perror("");
}
