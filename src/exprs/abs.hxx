#ifndef __ABS__H__
#define __ABS__H__

#include "expr.hxx"
#include <unordered_map>

class ExprAbs : public Expr
{
  public:
	ExprAbs() = delete;
	ExprAbs(const std::string &param, Expr *body);
	~ExprAbs();

	void CollectFree(std::unordered_set<std::string> &bound, std::unordered_set<std::string> &free) const override;

	Expr *Substitute(const std::string &var, const Expr *expr) override;

	Expr *Duplicate() const override;

	Expr *Reduce() override;

	Expr *Call(const Expr *param) override;

  private:
	std::string param;
	Expr *body;
	std::string ToString() const override;

	void Disambiguate();
};
#endif //!__ABS__H__
