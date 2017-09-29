#ifndef LAB3_WORD_COUNT_H
#define LAB3_WORD_COUNT_H

#include <string>

/**
 * Counts number of words, separated by spaces, in a line.
 * @param line string in which to count words
 * @param start_idx starting index to search for words
 * @return number of words in the line
 */
int word_count(const std::string& line, int start_idx);

class WordCountError : public std::exception {
	std::string message;

public:
	WordCountError(std::string _message) {
		message = _message;
	}
	const char* what() {
		return "Error using WordCount";
	}

	std::string info() {
		std::string out;
		out.append(message);
		return out;
	}

	std::string getMessage() {
		return message;
	}

};
#endif //LAB3_WORD_COUNT_H