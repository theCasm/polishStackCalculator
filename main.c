#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>

#define true 1
#define false 0

#define SBUFFERSIZE 512
#define CBUFFERSIZE 4096

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
struct term peek(void);
struct term nextTerm(char *s, size_t start);

int main()
{
	char input[CBUFFERSIZE];
	fgets(input, CBUFFERSIZE, stdin);

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

struct term peek()
{
	if (STACK_POINTER == 0) {
		fputs("peek() called on empty stack\n", stderr);
		exit(1);
	}
	return STACK[STACK_POINTER - 1];
}

struct term nextTerm(char *s, size_t start)
{
	struct term ans;
	while (isspace(s[start])) ++start;

	if (s[start] == '\0') {
		fputs("Term Search Overflows string\n", stderr);
		exit(1);
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
	--ans.end;

	// ensure '-' isnt treated as a number
	if (ans.end == ans.start && s[ans.start] == '-') {
		ans.isNum = false;
	}

	if (ans.isNum) {
		ans.val = strtod(s + ans.start, NULL);
	}

	return ans;
}
