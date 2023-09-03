#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define true 1
#define false 0

#define SBUFFERSIZE 512
#define CBUFFERSIZE 4096

#define opcmp(s, op, str) (strncmp(s + op.start, str, op.end - op.start) == 0)

//#define printT(t) (printf("%d %d %lf %d\n", t.start, t.end, t.val, t.isNum))

/*
 * start should be the index of the first char belonging to the term
 * end should be the index of the first char *not* belonging to the term
 * if a number, num should be its value
*/
struct term {
	size_t start;
	size_t end;
	double val;
	int isNum;
};

static struct term STACK[SBUFFERSIZE];
static size_t STACK_POINTER;

void push(struct term t);
struct term pop(void);
struct term peek(unsigned int num);
struct term nextTerm(char *s, size_t start);
void condense(char *s);

int main()
{
	struct term current;
	size_t index;
	char input[CBUFFERSIZE];
	fgets(input, CBUFFERSIZE, stdin);

	index = 0;
	while ((current = nextTerm(input, index)).isNum != -1) {
		push(current);

		while (peek(0).isNum && peek(1).isNum) {
			condense(input);
		}

		index = current.start + 1;
	}
	current = pop();
	printf("%lf\n", current.val);

	return 0;
}

void push(struct term t)
{
	if (STACK_POINTER >= SBUFFERSIZE) {
		fputs("Stack Buffer Overflow\n", stderr);
	}
	STACK[STACK_POINTER++] = t;
}

struct term pop()
{
	if (STACK_POINTER == 0) {
		fputs("pop() called on empty stack\n", stderr);
		exit(1);
	}
	return STACK[--STACK_POINTER];
}

struct term peek(unsigned int num)
{
	if (STACK_POINTER == 0) {
		fputs("peek() called on empty stack\n", stderr);
		exit(1);
	}
	return STACK[STACK_POINTER - 1 - num];
}

struct term nextTerm(char *s, size_t start)
{
	struct term ans;
	while (isspace(s[start])) ++start;

	if (s[start] == '\0') {
		// return weird term if at end
		ans.isNum = -1;
		return ans;
	}
	ans.start = start;

	ans.isNum = true;
	if (!isdigit(s[start]) && s[start] != '-' && s[start] != '.') {
		ans.isNum = false;
	}

	ans.end = start + 1;
	while (!isspace(s[ans.end]) && s[ans.end] != '\0') {
		if (!isdigit(s[ans.end]) && s[ans.end] != '.' && s[ans.end] != 'e'
		&& s[ans.end] != 'E') {
			ans.isNum = false;
		}

		++ans.end;
	}

	// ensure '-' isnt treated as a number
	if (ans.end == ans.start + 1 && s[ans.start] == '-') {
		ans.isNum = false;
	}

	if (ans.isNum) {
		ans.val = strtod(s + ans.start, NULL);
	}

	return ans;
}

void condense(char *s)
{
	struct term n1, n2, lastOp;

	n2 = pop();
	n1 = pop();
	lastOp = pop();
	lastOp.isNum = true;

	if (opcmp(s, lastOp, "+")) {
		lastOp.val = n1.val + n2.val;
	} else if (opcmp(s, lastOp, "-")) {
		lastOp.val = n1.val - n2.val;
	} else if (opcmp(s, lastOp, "*")) {
		lastOp.val = n1.val * n2.val;
	} else if (opcmp(s, lastOp, "/")) {
		lastOp.val = n1.val / n2.val;
	} else {
		// prepare to error out - desecrate s to print
		// message properly
		s[lastOp.end] = '\0';
		fprintf(stderr, "Unknown Operation: %s\n", s + lastOp.start);
	}
	push(lastOp);
}
