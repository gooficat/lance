/* Lambda calculus implementation in plain C90*/

#include <ctype.h>
#include <stdint.h>
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
			struct Expr *parent;
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
		Expr_print(self->app.b);
		putchar(')');
		break;
	default:
		puts("Nil");
		break;
	}
}

const char *Expr_parse(struct Expr *expr, const char *s, struct Expr *parent)
{
	switch (*s)
	{
	case '\\':
		expr->type = EXPR_ABS;
		++s;
		expr->abs.name = *s;
		expr->abs.expr = malloc(sizeof *expr->abs.expr);
		s += 2;
		s = Expr_parse(expr->abs.expr, s, expr);
		expr->abs.parent = parent;
		break;
	case '(':
		expr->type = EXPR_APP;
		expr->app.a = malloc(sizeof *expr->app.a);
		expr->app.b = malloc(sizeof *expr->app.b);
		++s;
		s = Expr_parse(expr->app.a, s, parent);
		s = Expr_parse(expr->app.b, s, parent);
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
		content = Expr_parse(expr, content, NULL);
	}
	else
	{
		expr->type = EXPR_NIL;
	}

	return expr;
}

struct Expr *deep_clone(struct Expr *expr)
{
	struct Expr *out = malloc(sizeof *out);
	out->type = expr->type;
	switch (expr->type)
	{
	case EXPR_VAR:
		out->var = expr->var;
		break;
	case EXPR_ABS:
		out->abs.name = expr->abs.name;
		out->abs.parent = expr->abs.parent;
		out->abs.expr = deep_clone(expr->abs.expr);
		break;
	case EXPR_APP:
		out->app.a = deep_clone(expr->app.a);
		out->app.b = deep_clone(expr->app.b);
	default:
		break;
	}
	return out;
}

struct Expr *substitute(struct Expr *expr, char v, struct Expr *sub)
{
	struct Expr *old = expr;
	switch (expr->type)
	{
	case EXPR_ABS:
		if (expr->abs.name != v)
			expr->abs.expr = substitute(expr->abs.expr, v, sub);
		free(old);
		return expr;
	case EXPR_APP:
		substitute(expr->app.a, v, sub);
		substitute(expr->app.b, v, sub);
	case EXPR_VAR:
		if (expr->var == v)
		{
			free(old);
			return deep_clone(sub);
		}
	default:
		return expr;
	}
}

struct Expr *beta_reduction(struct Expr *expr)
{
	if (expr->type == EXPR_APP)
		return substitute(expr->app.a->abs.expr, expr->app.a->abs.name, expr->app.b);
	return expr;
}

int main(int argc, char **argv)
{
	struct Expr *expr;
	// struct Expr *pool[UINT8_MAX] = {0};
	if (2 != argc)
	{
		fatal("No input file");
	}
	puts("Welcome to lance, a lambda calculus interpreter");
	printf("Now running `%s`...\n", argv[1]);

	expr = read_program(argv[1]);

	Expr_print(expr);
	putchar('\n');
	expr = beta_reduction(expr);
	Expr_print(expr);

	return 0;
}
