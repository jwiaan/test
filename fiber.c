#include <stdio.h>

#define save(m) asm("push %%rdi;push %%rbx;push %%rbp;push %%r12;push %%r13;push %%r14;push %%r15;mov %%rsp,%0":"=m"(m))
#define load(m) asm("mov %0,%%rsp;pop %%r15;pop %%r14;pop %%r13;pop %%r12;pop %%rbp;pop %%rbx;pop %%rdi;ret"::"m"(m))

enum { N = 510 };

struct node {
	void *p;
	struct node *next;
} node = {.next = &node }, *prev = &node;

struct fiber {
	struct node node;
	void *stack[N];
};

struct queue {
	struct node *head, **tail;
};

struct channel {
	struct queue data, wait;
} channel = {.data.tail = &channel.data.head,.wait.tail = &channel.wait.head };

int empty(const struct queue *q)
{
	return !q->head;
}

void push(struct queue *q, struct node *n)
{
	n->next = NULL;
	*q->tail = n;
	q->tail = &n->next;
}

struct node *pop(struct queue *q)
{
	struct node *n = q->head;
	if (!(q->head = n->next))
		q->tail = &q->head;

	return n;
}

void insert(struct node *n)
{
	struct node *curr = prev->next;
	n->next = curr->next;
	curr->next = n;
	if (curr == prev)
		prev = n;
}

struct node *erase(void)
{
	struct node *curr = prev->next;
	prev->next = curr->next;
	return curr;
}

__attribute__((naked))
void Switch(struct node *curr, struct node *next)
{
	save(curr->p);
	load(next->p);
}

void yield(void)
{
	prev = prev->next;
	Switch(prev, prev->next);
}

void wait(struct queue *q)
{
	struct node *curr = erase();
	push(q, curr);
	Switch(curr, prev->next);
}

void notify(struct queue *q)
{
	insert(pop(q));
	yield();
}

struct node *get(struct channel *c)
{
	if (empty(&c->data))
		wait(&c->wait);

	return pop(&c->data);
}

void put(struct channel *c, struct node *n)
{
	push(&c->data, n);
	if (!empty(&c->wait))
		notify(&c->wait);
}

void end(void)
{
	struct node *curr = erase();
	Switch(curr, prev->next);
}

void init(struct fiber *f, void (*rip)(void *), void *rdi)
{
	f->stack[N - 1] = end;
	f->stack[N - 2] = rip;
	f->stack[N - 3] = rdi;
	f->node.p = &f->stack[N - 9];
	insert(&f->node);
}

void test(void *p)
{
	printf("%d %p\n", __LINE__, p);
	const char *s = get(&channel)->p;
	printf("%d %p %s\n", __LINE__, p, s);
}

int main(void)
{
	struct fiber f1, f2;
	init(&f2, test, (void *)2);
	init(&f1, test, (void *)1);
	printf("%d\n", __LINE__);
	yield();
	printf("%d\n", __LINE__);

	struct node n1 = { "aaa" }, n2 = { "bbb" };
	printf("%d\n", __LINE__);
	put(&channel, &n1);
	printf("%d\n", __LINE__);
	put(&channel, &n2);
	printf("%d\n", __LINE__);
}
