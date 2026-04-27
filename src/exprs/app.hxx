#ifndef __APP__H__
#define __APP__H__

#include "expr.hxx"

class ExprApp : public Expr
{
  public:
	ExprApp() = delete;
	ExprApp(Expr *caller, Expr *callee);
	~ExprApp();

	void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const override;

	Expr *Substitute(const std::string &var, const Expr *expr) override;

	Expr *Duplicate() const override;
	Expr *Reduce() override;

	Expr *Call(const Expr *param) override;

  private:
	Expr *caller;
	Expr *callee;
	std::string ToString() const override;
};

#endif //!__APP__H__
