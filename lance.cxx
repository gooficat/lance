

#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <print>
#include <string>
#include <string_view>

class Expr
{
  public:
	virtual std::string AsString() const
	{
		return "test!";
	}
};

template <>
struct std::formatter<Expr> : public std::formatter<std::string>
{
	auto format(const Expr &self, std::format_context &format_ctx) const
	{
		return std::formatter<std::string>::format(self.AsString(), format_ctx);
	}
};

class ExprVar : public Expr
{
  public:
	ExprVar() = delete;
	ExprVar(const std::string &name) : name(name)
	{
	}

	~ExprVar()
	{
		if (value.has_value())
			delete value.value();
	}

  private:
	std::string name;
	std::optional<Expr *> value;
	virtual std::string AsString() const override
	{
		return name;
	}
};

class ExprAbs : public Expr
{
  public:
	ExprAbs() = delete;
	ExprAbs(const std::string &param, Expr *body) : param(param), body(body)
	{
	}
	~ExprAbs()
	{
		delete body;
	}

  private:
	std::string param;
	Expr *body;
	virtual std::string AsString() const override
	{
		return std::format("λ{}.{}", param, body->AsString());
	}
};

class ExprApp : public Expr
{
  public:
	ExprApp() = delete;
	ExprApp(Expr *caller, Expr *callee) : caller(caller), callee(callee)
	{
	}
	~ExprApp()
	{
		delete caller;
		delete callee;
	}

  private:
	Expr *caller;
	Expr *callee;
	virtual std::string AsString() const override
	{
		return std::format("({} {})", *caller, *callee);
	}
};

Expr *ParseExpr(std::string_view &v)
{
	Expr *out;
	switch (v[0])
	{
	case '\\':
	{
		auto param = std::string(v.substr(1, 1));
		v.remove_prefix(3);
		out = new ExprAbs(param, ParseExpr(v));
		break;
	}
	case '(':
	{
		v.remove_prefix(1);
		auto left = ParseExpr(v);
		auto right = ParseExpr(v);
		v.remove_prefix(1);
		out = new ExprApp(left, right);
		break;
	}
	default:
	{
		out = new ExprVar(std::string(v.substr(0, 1)));
		v.remove_prefix(1);
		break;
	}
	}
	return out;
}

int main()
{
	std::string content = "(\\x.xy)";
	std::string_view view = content;
	auto tree = ParseExpr(view);
	std::println("{}", *tree);
	delete tree;
	return 0;
}
