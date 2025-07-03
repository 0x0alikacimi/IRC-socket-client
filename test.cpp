#include <iostream>
#include <vector>

std::vector<std::string> splitBySpace(const std::string& input) {
	std::vector<std::string> tokens;
	std::string token;
	for (size_t i = 0; i < input.length(); ++i) {
		if (input[i] == ' ' || input[i] == '\t') {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
		} else {
			token += input[i];
		}
	}
	if (!token.empty()) {
		tokens.push_back(token);
	}
	return tokens;
}

int main(){
	std::string buff = "     hi 			you are   beuty but         ";
	std::vector<std::string> tokens = splitBySpace(buff);
	for (std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
    	std::cout << *it << std::endl;
}
}
