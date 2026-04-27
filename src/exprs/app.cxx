#include "app.hxx"
ExprApp::~ExprApp()
{
	// if (caller)
	// delete caller;
	// if (callee)
	// delete callee;
}
ExprApp::ExprApp(Expr *caller, Expr *callee) : caller(caller), callee(callee)
{
}
void ExprApp::CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const
{
	caller->CollectFree(bound, free);
	callee->CollectFree(bound, free);
}
Expr *ExprApp::Substitute(const std::string &var, const Expr *expr)
{
	// delete this;
	return new ExprApp(caller->Substitute(var, expr),
					   callee->Substitute(var, expr));
}
Expr *ExprApp::Duplicate() const
{
	return new ExprApp(caller->Duplicate(), callee->Duplicate());
}
Expr *ExprApp::Reduce()
{
	auto new_expr = caller->Call(callee);
	// delete this;
	return new_expr;
}
Expr *ExprApp::Call(const Expr *param)
{
	return this;
}
std::string ExprApp::ToString() const
{
	return std::format("({} {})", *caller, *callee);
}
