#include <stdio.h>

enum { N = 1024 };

struct node {
	void *rsp;
	struct node *next;
} head = {.next = &head }, *prev = &head;

struct fiber {
	void *stack[N];
	struct node node;
};

#define save(m) asm("push %%rdi;push %%rbx;push %%r12;push %%r13;push %%r14;push %%r15;mov %%rsp,%0":"=m"(m))
#define load(m) asm("mov %0,%%rsp;pop %%r15;pop %%r14;pop %%r13;pop %%r12;pop %%rbx;pop %%rdi"::"m"(m))

void yield(void)
{
	save(prev->next->rsp);
	prev = prev->next;
	load(prev->next->rsp);
}

void clean(void)
{
	prev->next = prev->next->next;
	load(prev->next->rsp);
}

void insert(struct node *n)
{
	n->next = prev->next;
	prev->next = n;
	prev = n;
}

void init(struct fiber *f, void (*rip)(void *), void *rdi)
{
	f->stack[N - 1] = clean;
	f->stack[N - 2] = rip;
	f->stack[N - 4] = rdi;
	f->node.rsp = &f->stack[N - 9];
	insert(&f->node);
}

void test(void *p)
{
	printf("%d %p\n", __LINE__, p);
	yield();
	printf("%d %p\n", __LINE__, p);
}

int main(void)
{
	struct fiber f1, f2;
	init(&f1, test, (void *)1);
	init(&f2, test, (void *)2);
	printf("%d\n", __LINE__);
	yield();
	printf("%d\n", __LINE__);
	yield();
	printf("%d\n", __LINE__);
	yield();
	printf("%d\n", __LINE__);
}
