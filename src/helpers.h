#pragma once
#include <functional>
#include <vector>

//Contain helper methods
namespace Helpers {
	//Joins the given values using the given seperator
	template <class T>
	std::string join(const std::vector<T>& values, std::function<std::string(T)> toString, std::string sep) {
		bool isFirst = true;
		std::string str = "";

		for (auto val : values) {
			if (!isFirst) {
				str += sep;
			} else {
				isFirst = false;
			}

			str += toString(val);
		}

		return str;
	}
};