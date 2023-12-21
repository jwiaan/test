#include <stdio.h>
#include <stdlib.h>

struct node {
	int i;
	struct node *next;
};

void erase(struct node **prev, int i)
{
	for (struct node * curr = *prev; curr; curr = *prev) {
		if (curr->i == i) {
			*prev = curr->next;
			free(curr);
		} else {
			prev = &curr->next;
		}
	}
}

void insert(struct node **prev, int i)
{
	struct node *n = malloc(sizeof(struct node));
	n->i = i;
	n->next = *prev;
	*prev = n;
}

void print(const struct node *n)
{
	while (n) {
		printf("%d ", n->i);
		n = n->next;
	}
	printf("\n");
}

int main(void)
{
	struct node *head = NULL;
	int n = 10;
	for (int i = 0; i < n; i++)
		insert(&head, i);

	print(head);
	for (int i = 0; i < n + 1; i++) {
		erase(&head, i);
		print(head);
	}
}
