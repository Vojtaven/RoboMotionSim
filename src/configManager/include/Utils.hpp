#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <sstream>
#include "silent_exprtk.hpp"
class Utils {
public:
	static std::vector<std::string> split(const std::string& s, char delimiter);
};

template<typename T = float>
class ExprEvaluator {
private:
	exprtk::symbol_table<T> _symbolTable;
	exprtk::expression<T> _expression;
	exprtk::parser<T> _parser;
public:
	ExprEvaluator() {
		// Register standard constants (like 'pi', 'e')
		_symbolTable.add_constants();
		_expression.register_symbol_table(_symbolTable);
	}

	T evaluateExpressions(const std::string& formula) {
		if (!_parser.compile(formula, _expression)) {
			std::string error_message = "ExprTk Parsing Error in formula: '" + formula + "'. Details:\n";

			for (std::size_t i = 0; i < _parser.error_count(); ++i) {
				const auto& error = _parser.get_error(i);
				error_message += "  - Error at position " + std::to_string(error.token.position) +
					": " + error.diagnostic + "\n";
			}

			throw std::invalid_argument(error_message);
		}

		return _expression.value();
	}
};
#endif // !UTILS_HPP
