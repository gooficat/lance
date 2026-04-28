#include "expr.hxx"
#include "abs.hxx"
#include "app.hxx"
#include "var.hxx"

std::unordered_set<std::string> Expr::GetFree() const
{
	std::unordered_set<std::string> bound, free;
	CollectFree(bound, free);
	return free;
}

Expr::~Expr()
{
}

Expr *ParseExpr(std::string_view &v)
{
	switch (v[0])
	{
	case '\\':
	{
		auto param = std::string(v.substr(1, 1));
		v.remove_prefix(3);
		return new ExprAbs(param, ParseExpr(v));
	}
	case '(':
	{
		v.remove_prefix(1);
		auto left = ParseExpr(v);
		auto right = ParseExpr(v);
		v.remove_prefix(1);
		return new ExprApp(left, right);
	}
	default:
	{
		auto out = new ExprVar(std::string(v.substr(0, 1)));
		v.remove_prefix(1);
		return out;
	}
	}
}
