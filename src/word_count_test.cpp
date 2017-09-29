#include "word_count.h"
#include <exception>
#include <iostream>

// Exception class to throw for unit test failures
class UnitTestException : public std::exception {
  std::string line_;
  int start_idx_;
  int result_;
  int expected_;

 public:

  // constructor collecting all information
  UnitTestException(const std::string& line, int start_idx,
                    int result, int expected) : line_(line), start_idx_(start_idx),
                                                result_(result), expected_(expected) {}

  const char* what() {
    return "Unit test failed";
  }

  std::string info() {
    std::string out;
    out.append("line: ");
    out.append(line_);
    out.append(", start_idx: ");
    out.append(std::to_string(start_idx_));
    out.append(", result: ");
    out.append(std::to_string(result_));
    out.append(", expected: ");
    out.append(std::to_string(expected_));
    return out;
  }

};

/**
 * Tests word_count for the given line and starting index
 * @param line line in which to search for words
 * @param start_idx starting index in line to search for words
 * @param expected expected answer
 * @throws UnitTestException if the test fails
 */
void wc_tester(const std::string& line, int start_idx, int expected) {

  int result = word_count(line, start_idx);

  // if not what we expect, throw an error
  if (result != expected) {
    throw UnitTestException(line, start_idx, result, expected);
  }

}

void wc_tester(const std::string& line, int start_idx, std::string error_message) {
	bool correct_result = false;
	int result = 0;

	try {
		result = word_count(line, start_idx);
	}
	catch (WordCountError & thrown_error) {
		if (error_message.compare(thrown_error.getMessage()) == 0)
			correct_result = true;
	}

	if (!correct_result) {
		throw UnitTestException(line, start_idx, result, 0);
	}

}

int main() {


	
  try {

    // YOUR TESTS HERE
    wc_tester("hello world", 0, 2);
	wc_tester("hello world", 50, "index out of bounds");
	wc_tester("   hello weold  %# ", 0, 2);
	wc_tester("      @#$@%# ", 0, 0);
	wc_tester("there are 4 words here", 0, 4);
	wc_tester("there are 4 words here", 5, 3);
	wc_tester("there are 4 words here", 9, 2);
	wc_tester("there are 4 words here", 17, 1);


	wc_tester("", 0, 0);

  } catch(UnitTestException &ute) {
    std::cout << ute.info() << std::endl;
  }


  std::cout << "All Unit Tests Passed!!" << std::endl;
  while (true) {};

}