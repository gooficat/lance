

#include <cstddef>
#include <format>
#include <fstream>
#include <optional>
#include <print>
#include <sstream>
#include <stdexcept>
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

	std::unordered_set<std::string> GetFree() const
	{
		std::unordered_set<std::string> bound, free;
		CollectFree(bound, free);
		return free;
	}

	virtual void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const = 0;

	virtual Expr *Substitute(const std::string &var, const Expr *expr) = 0;

	virtual Expr *Duplicate() const = 0;

	virtual Expr *Reduce() = 0;

	virtual Expr *Call(const Expr *param) = 0;
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
		// if (value.has_value())
		// delete value.value();
	}

	void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const override
	{
		if (not bound.contains(name))
		{
			free.insert(name);
		}
	}

	Expr *Substitute(const std::string &var, const Expr *expr) override
	{
		if (this->name == var)
		{
			// delete this;
			return expr->Duplicate();
		}
		return this;
	}

	Expr *Duplicate() const override
	{
		return new ExprVar(name);
	}

	Expr *Reduce() override
	{
		return this;
	}

	Expr *Call(const Expr *param) override
	{
		return this;
	}

  private:
	std::string name;
	std::optional<Expr *> value;
	std::string ToString() const override
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
		// if (body)
		// delete body;
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

	Expr *Substitute(const std::string &var, const Expr *expr) override
	{
		if (param == var)
		{
			return this;
		}
		if (GetFree().contains(var))
		{
			Disambiguate();
			auto new_body = body->Substitute(var, expr);
			// delete this;
			return new ExprAbs(param, new_body);
		}
		body->Substitute(var, expr);
	}

	Expr *Duplicate() const override
	{
		return new ExprAbs(param, body->Duplicate());
	}

	Expr *Reduce() override
	{
		// TODO
		return this;
	}

	Expr *Call(const Expr *param) override
	{
		return body->Substitute(this->param, param);
	}

  private:
	std::string param;
	Expr *body;
	std::string ToString() const override
	{
		return std::format("λ{}.{}", param, body->ToString());
	}

	void Disambiguate()
	{
		static std::unordered_map<char, size_t> counts;
		char c = param[0];
		size_t v;
		if (counts.contains(c))
		{
			v = counts.at(c)++;
		}
		else
		{
			counts.insert({c, 1});
			v = 1;
		}
		param = std::format("{}{}", c, v);
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
		// if (caller)
		// delete caller;
		// if (callee)
		// delete callee;
	}

	void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const override
	{
		caller->CollectFree(bound, free);
		callee->CollectFree(bound, free);
	}

	Expr *Substitute(const std::string &var, const Expr *expr) override
	{
		// delete this;
		return new ExprApp(caller->Substitute(var, expr),
						   callee->Substitute(var, expr));
	}

	Expr *Duplicate() const override
	{
		return new ExprApp(caller->Duplicate(), callee->Duplicate());
	}
	Expr *Reduce() override
	{
		auto new_expr = caller->Call(callee);
		// delete this;
		return new_expr;
	}

	Expr *Call(const Expr *param) override
	{
		return this;
	}

  private:
	Expr *caller;
	Expr *callee;
	std::string ToString() const override
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

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		throw std::runtime_error("Wrong args");
	}
	std::ifstream in(argv[1]);
	std::stringstream insstrm;
	insstrm << in.rdbuf();
	in.close();
	std::string content = insstrm.str();
	std::string_view view = content;
	auto tree = ParseExpr(view);
	std::println("{}", *tree);
	auto new_tree = tree->Reduce();
	while (tree != new_tree)
	{
		tree = new_tree;
		new_tree = tree->Reduce();
	}
	std::println("{}", *tree);
	// delete tree;
	return 0;
}
