#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <sstream>
#include <numbers>
#include "silent_exprtk.hpp"
class Utils {
public:
	static std::vector<std::string> split(const std::string& s, char delimiter);
	static inline float DegreesToRadians(float degrees) {
		return (float)(degrees * (std::numbers::pi / 180));
	}

	static inline float RadiansToDegree(float radians) {
		return (float)(radians / (std::numbers::pi / 180));
	}
};

template<typename T = float>
class ExprEvaluator {
private:
	exprtk::symbol_table<T> g_symbol_table;
	exprtk::expression<T> g_expression;
	exprtk::parser<T> g_parser;
public:
	ExprEvaluator() {
		// Register standard constants (like 'pi', 'e')
		g_symbol_table.add_constants();
		g_expression.register_symbol_table(g_symbol_table);
	}

	T EvaluateExpressions(const std::string& formula) {
		if (!g_parser.compile(formula, g_expression)) {
			std::string error_message = "ExprTk Parsing Error in formula: '" + formula + "'. Details:\n";

			for (std::size_t i = 0; i < g_parser.error_count(); ++i) {
				const auto& error = g_parser.get_error(i);
				error_message += "  - Error at position " + std::to_string(error.token.position) +
					": " + error.diagnostic + "\n";
			}

			throw std::runtime_error(error_message);
		}

		return g_expression.value();
	}
};
#endif // !UTILS_HPP
