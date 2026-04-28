#include "abs.hxx"
void ExprAbs::Disambiguate()
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
		counts.insert({c, 2});
		v = 1;
	}
	param = std::format("{}{}", c, v);
}
std::string ExprAbs::ToString() const
{
	return std::format("λ{}.{}", param, body->ToString());
}
Expr *ExprAbs::Call(const Expr *param)
{
	return body->Substitute(this->param, param);
}
Expr *ExprAbs::Reduce()
{
	// TODO
	return this;
}
Expr *ExprAbs::Duplicate() const
{
	return new ExprAbs(param, body->Duplicate());
}
Expr *ExprAbs::Substitute(const std::string &var, const Expr *expr)
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
	body = body->Substitute(var, expr);
	return this;
}
void ExprAbs::CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const
{
	auto need_to_remove = bound.contains(param);
	if (!need_to_remove)
		bound.insert(param);
	body->CollectFree(bound, free);
	if (need_to_remove)
		bound.erase(param);
}
ExprAbs::~ExprAbs()
{
	// if (body)
	// delete body;
}
ExprAbs::ExprAbs(const std::string &param, Expr *body) : param(param), body(body)
{
}
