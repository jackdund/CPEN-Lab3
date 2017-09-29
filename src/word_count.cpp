#include "word_count.h"
#include <vector>




// implementation details
int word_count(const std::string& line, int start_idx) {

  //check string is not null
	if (line.length() == 0)
		return 0;

	//ensure start_idx is within string bounds
	if (start_idx >= line.length()) 
		throw WordCountError("index out of bounds");
	
		

  int nwords = 0;
  bool word_start = false;

  for (int i = start_idx; i < line.length(); ++i) {
	  
	  if (::isalpha(line[i]) != 0 && !word_start) {
		  word_start = true;
		  nwords++;
	  }

	  if (::isalpha(line[i]) == 0)
		  word_start = false;  
  }


  return nwords;
  }

//while index is less than the size of the string
//mark the start of a word
//mark the end of a word
//increase word count by one and reset variables
/*
int index = start_idx;
bool word_start = false;
bool word_end = false;

while (index < index_of_non_char.size()) {

	//mark the start of a word*/




