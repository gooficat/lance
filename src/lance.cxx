#include "exprs/expr.hxx"
#include <fstream>
#include <print>
#include <sstream>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		throw std::runtime_error("Wrong args");
	}
	std::ifstream in(argv[1]);
	if (!in.is_open())
	{
		throw std::runtime_error("Failed to open file");
	}
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
