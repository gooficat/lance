#include "var.hxx"

ExprVar::ExprVar(const std::string &name) : name(name)
{
}
ExprVar::~ExprVar()
{
}
void ExprVar::CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const
{
	if (not bound.contains(name))
	{
		free.insert(name);
	}
}
Expr *ExprVar::Substitute(const std::string &var, const Expr *expr)
{
	if (this->name == var)
	{
		// delete this;
		return expr->Duplicate();
	}
	return this;
}
Expr *ExprVar::Duplicate() const
{
	return new ExprVar(name);
}
Expr *ExprVar::Reduce()
{
	return this;
}
Expr *ExprVar::Call(const Expr *param)
{
	return this;
}
std::string ExprVar::ToString() const
{
	return name;
}
