#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "assemble.h"

static uint32_t PC;

struct tok
{
	const char *tok;
	struct tok *next;
};

struct op_to_func
{
	const char *op;
	unsigned char opcode;
	int (*f)(int, struct tok *);
};

static int op_r(int, struct tok *);
static int op_imm(int, struct tok *);
static int op_r_r(int, struct tok *);
static int op_imm_r(int, struct tok *);

static struct op_to_func opmap[] =
{
	{"jmp",  2, op_r},
	{"jmp",  1, op_imm},
	{"mov",  3, op_imm_r},
	{"mov",  4, op_r_r},
	{"wrt", 10, op_r_r},
	{"wrt", 11, op_imm_r},
	{"add", 12, op_imm_r},
	{"add",  5, op_r_r},
	{"sub", 13, op_imm_r},
	{"sub",  6, op_r_r},
	{"and", 14, op_imm_r},
	{"and",  7, op_r_r},
	{"or",  15, op_imm_r},
	{"or",   8, op_r_r},
	{"xor", 16, op_imm_r},
	{"xor",  9, op_r_r},
	{"lsh", 17, op_imm_r},
	{"lsh", 21, op_r_r},
	{"rsh", 18, op_imm_r},
	{"rsh", 22, op_r_r},
	{"rl",  19, op_imm_r},
	{"rl",  23, op_r_r},
	{"rr",  20, op_imm_r},
	{"rr",  24, op_r_r},
	{NULL,   0, NULL}
};

static int issreg(const char *s)
{
	unsigned long r;

	if(s[0] != 'r')
		return 0;

	r = strtoul(s+1, NULL, 10);
	if(r > 15)
		return 0;

	return 1;
}

static int op_imm(int i, struct tok *t)
{
	int32_t off;

	if(!t)
		return 0;

	off = strtol(t->tok, NULL, 0);
	printf("%08X: %08X: %s 0x%X\n", PC, (off<<8)|1, opmap[i].op, (int32_t)PC+4+off);

	return 1;
}

static int op_r(int i, struct tok *t)
{
	if(!t)
		return 0;

	if(!issreg(t->tok))
		return 0;

	printf("%08X: %08X: %s %s\n",
		PC, (uint32_t)(strtoul(t->tok+1, NULL, 10)<<8) | 2, opmap[i].op, t->tok);

	return 1;
}

static int op_imm_r(int i, struct tok *t)
{
	int32_t imm, instr;

	if(!t || !issreg(t->tok))
		return 0;

	if(!t->next)
		return 0;

	imm = strtol(t->next->tok, NULL, 0);
	instr = imm << 12 | strtol(t->tok+1, NULL, 10)<<8 | opmap[i].opcode;
	printf("%08X: %08X: %s %s, 0x%X\n", PC, instr, opmap[i].op, t->tok, imm);

	return 1;
}

static int op_r_r(int i, struct tok *t)
{
	int32_t instr;

	if(!t || !issreg(t->tok))
		return 0;

	if(!t->next || !issreg(t->next->tok))
		return 0;

	instr = strtol(t->tok+1, NULL, 10) << 12 | strtol(t->next->tok+1, NULL, 10) << 8 | opmap[i].opcode;
	printf("%08X: %08X: %s %s, %s\n", PC, instr, opmap[i].op, t->tok, t->next->tok);

	return 1;
}

static void parse_line(struct tok *list)
{
	int i = 0;

	while(1)
	{
		if(!opmap[i].op)
			break;

		if(strcmp(opmap[i].op, list->tok) == 0)
			if(opmap[i].f(i, list->next))
			{
				PC += 4;
				break;
			}
		i++;
	}
}

static int open_and_parse(const char *filename)
{

	FILE *f;
	char line[1024], *p;

	f = fopen(filename, "rb");
	if(!f)
		return 0;

	while(1)
	{
		struct tok *list = NULL;
		struct tok **tail = &list;

		if(!fgets(line, 1024, f))
			break;

		p = line;
		while(*p == ' ' || *p == '\t')
			p++;

		if(*p == '\n')
			continue;

		p = strtok(p, " \t\n");
		if(!p)
			break;
		do {
			struct tok *n;

			n = malloc(sizeof *n);
			n->tok = p;
			*tail = n;
			tail = &n->next;
		} while((p = strtok(NULL, ", \t\n")));

		parse_line(list);
	}

	fclose(f);

	return 1;
}

int assemble(const char *filename)
{
	return open_and_parse(filename);
}
