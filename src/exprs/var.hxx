#ifndef __VAR__H__
#define __VAR__H__

#include "expr.hxx"

class ExprVar : public Expr
{
  public:
	ExprVar() = delete;
	ExprVar(const std::string &name);

	~ExprVar();

	void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const override;

	Expr *Substitute(const std::string &var, const Expr *expr) override;

	Expr *Duplicate() const override;

	Expr *Reduce() override;

	Expr *Call(const Expr *param) override;

  private:
	std::string name;
	std::string ToString() const override;
};

#endif //!__VAR__H__
