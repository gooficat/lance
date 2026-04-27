

#include <cstddef>
#include <format>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

class Context
{
  public:
	std::unordered_map<std::string, class Expr> vars;
};

class Expr
{
  public:
	virtual std::string ToString() const = 0;
	virtual ~Expr()
	{
	}

	virtual void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const = 0;

	virtual void Substitute(const std::string &var, const Expr *expr) = 0;
};

template <>
struct std::formatter<Expr> : public std::formatter<std::string>
{
	auto format(const Expr &self, std::format_context &format_ctx) const
	{
		return std::formatter<std::string>::format(self.ToString(), format_ctx);
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

	void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const override
	{
		if (not bound.contains(name))
		{
			free.insert(name);
		}
	}

	void Substitute(const std::string &var, const Expr *expr) override
	{
		if (this->name == var)
		{
			//
		}
	}

  private:
	std::string name;
	std::optional<Expr *> value;
	virtual std::string ToString() const override
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

	void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const override
	{
		auto need_to_remove = bound.contains(param);
		if (!need_to_remove)
			bound.insert(param);
		body->CollectFree(bound, free);
		if (need_to_remove)
			bound.erase(param);
	}

	void Substitute(const std::string &var, const Expr *expr) override
	{
		if (param != var)
		{
			body->Substitute(var, expr);
		}
	}

  private:
	std::string param;
	Expr *body;
	virtual std::string ToString() const override
	{
		return std::format("λ{}.{}", param, body->ToString());
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

	void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const override
	{
		caller->CollectFree(bound, free);
		callee->CollectFree(bound, free);
	}

	void Substitute(const std::string &var, const Expr *expr) override
	{
		caller->Substitute(var, expr);
		callee->Substitute(var, expr);
	}

  private:
	Expr *caller;
	Expr *callee;
	virtual std::string ToString() const override
	{
		return std::format("({} {})", *caller, *callee);
	}
};

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

int main()
{
	std::string content = "(\\x.xy)";
	std::string_view view = content;
	auto tree = ParseExpr(view);
	std::println("{}", *tree);
	delete tree;
	return 0;
}
