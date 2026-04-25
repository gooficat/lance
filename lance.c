/* Lambda calculus implementation in plain C90*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define fatal(msg)                                                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		fprintf(stderr, "ERROR! %s\n", msg);                                                                           \
		exit(EXIT_FAILURE);                                                                                            \
	} while (0)

struct Expr
{
	enum ExprType
	{
		EXPR_NIL,
		EXPR_VAR,
		EXPR_ABS,
		EXPR_APP,
	} type;
	union
	{
		char var;
		struct ExprAbs
		{
			char name;
			struct Expr *expr;
		} abs;
		struct ExprApp
		{
			struct Expr *a;
			struct Expr *b;
		} app;
	};
};

void Expr_print(struct Expr *self)
{
	switch (self->type)
	{
	case EXPR_VAR:
		printf("%c", self->var);
		break;
	case EXPR_ABS:
		printf("\\%c.", self->abs.name);
		Expr_print(self->abs.expr);
		break;
	case EXPR_APP:
		putchar('(');
		Expr_print(self->app.a);
		putchar(' ');
		Expr_print(self->app.b);
		putchar(')');
	default:
		break;
	}
}

const char *Expr_parse(struct Expr *expr, const char *s)
{
	switch (*s)
	{
	case '\\':
		expr->type = EXPR_ABS;
		++s;
		expr->abs.name = *s;
		expr->abs.expr = malloc(sizeof *expr->abs.expr);
		s += 2;
		s = Expr_parse(expr->abs.expr, s);
		break;
	case '(':
		expr->type = EXPR_APP;
		expr->app.a = malloc(sizeof *expr->app.a);
		expr->app.b = malloc(sizeof *expr->app.b);
		++s;
		s = Expr_parse(expr->app.a, s);
		++s;
		s = Expr_parse(expr->app.b, s);
		++s;
		break;
	default:
		expr->type = EXPR_VAR;
		expr->var = *s++;
		break;
	}
	return s;
}

char *dump_file(const char *path)
{
	FILE *file;
	char *contents;
	long l;
	size_t rl;
#ifndef _WIN32
	file = fopen(path, "rt");
#else
	fopen_s(&file, path, "rt");
#endif
	if (!file)
	{
		fatal("Failure opening file for reading");
	}
	fseek(file, 0l, SEEK_END);
	l = ftell(file);
	rewind(file);
	contents = malloc(l + 1);
	if (!contents)
	{
		fatal("Failure allocating buffer for file contents");
	}
	rl = fread(contents, sizeof(char), l, file);
	if (rl != (size_t)l)
	{
		contents = realloc(contents, rl + 1);
		if (!contents)
		{
			fatal("Failure reallocating buffer for file contents");
		}
	}
	contents[rl] = '\0';
	return contents;
}

struct Expr *read_program(const char *path)
{
	struct Expr *expr;
	const char *content;
	content = dump_file(path);
	expr = malloc(sizeof *expr);

	if (*content != '\0')
	{
		content = Expr_parse(expr, content);
	}
	else
	{
		expr->type = EXPR_NIL;
	}

	return expr;
}

int main(int argc, char **argv)
{
	struct Expr *expr;
	if (2 != argc)
	{
		fatal("No input file");
	}
	puts("Welcome to lance, a lambda calculus interpreter");
	printf("Now running `%s`...\n", argv[1]);

	expr = read_program(argv[1]);

	Expr_print(expr);

	return 0;
}
