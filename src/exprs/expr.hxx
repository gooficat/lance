#ifndef __EXPR__H__
#define __EXPR__H__

#include <format>
#include <string>
#include <unordered_set>

class Expr
{
  public:
	virtual std::string ToString() const = 0;
	virtual ~Expr() = 0;

	std::unordered_set<std::string> GetFree() const;

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

Expr *ParseExpr(std::string_view &v);

#endif //!__EXPR__H__
