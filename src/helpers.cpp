#include "helpers.h"

std::vector<std::string> Helpers::splitString(std::string str, std::string delimiter) {
	std::vector<std::string> parts;

	std::size_t pos = 0;
	std::string token;
	while ((pos = str.find(delimiter)) != std::string::npos) {
	    token = str.substr(0, pos);
	    parts.push_back(token);
	    str.erase(0, pos + delimiter.length());
	}

	parts.push_back(str);

	return parts;
}

std::string Helpers::replaceString(std::string str, std::string search, std::string replace) {
	std::size_t index = 0;
	while (true) {
	     index = str.find(search, index);
	     if (index == std::string::npos) {
	     	break;
	     }

	     str.replace(index, search.size(), replace);
	}

	return str;
}