#include <map>
#include <string>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

#include "word_count.h"


//1) How can you test your program without needing to manually go through all the dialogue in Shakespeare’s plays ?
	//Verify a small section of dialogue and make sure it adds up. 

//2) Has writing this code multithreaded helped in any way ?
	//Yes -- 

// 3) As written, if a character in one play has the same name as a character in another – e.g.King – it will threat them as the same and artificially increase the word count.How can you modify your code to treat them as separate, but still store all characters in the single map(you do not need to implement this… just think about how you would do it) ?

/**
 * Checks if the line specifies a character's dialogue, returning
 * the index of the start of the dialogue.  If the
 * line specifies a new character is speaking, then extracts the
 * character's name.
 *
 * Assumptions: (doesn't have to be perfect)
 *     Line that starts with exactly two spaces has
 *       CHARACTER. <dialogue>
 *     Line that starts with exactly four spaces
 *       continues the dialogue of previous character
 *
 * @param line line to check
 * @param character extracted character name if new character,
 *        otherwise leaves character unmodified
 * @return index of start of dialogue if a dialogue line,
 *      -1 if not a dialogue line
 */
int is_dialogue_line(const std::string& line, std::string& character) {

  // extract character name and save it to the character reference
  if (line.length() >= 3 && line[0] == ' ' && line[1] == ' ' && line[2] != ' ') {
    // extract character name

    int start_idx = 2;
    int end_idx = 3;
    while (end_idx <= line.length() && line[end_idx-1] != '.') {
      ++end_idx;
    }

    // no name found
    if (end_idx >= line.length()) {
      return 0;
    }

    // extract character's name
    character = line.substr(start_idx, end_idx-start_idx-1);
    return end_idx;
  }

  // previous character
  if (line.length() >= 5 && line[0] == ' '
      && line[1] == ' ' && line[2] == ' '
      && line[3] == ' ' && line[4] != ' ') {
    // continuation
    return 4;
  }

  return 0;
}

/**
 * Reads a file to count the number of words each actor speaks.
 *
 * @param filename file to open
 * @param mutex mutex for protected access to the shared wcounts map
 * @param wcounts a shared map from character -> word count
 */
void count_character_words(const std::string& filename,
                           std::mutex& mutex,
                           std::map<std::string,int>& wcounts) {

  std::string line;  // for storing each line read from the file
  std::ifstream file (filename);

  // read contents of file if open
  if (file.is_open()) {

    std::string character = "";  // empty character to start

    // line by line
    while ( std::getline (file,line) ) {

      int idx = is_dialogue_line(line, character);
	  
	  //index will be -1 if the line is not a dialogue line, and character will be empty if there is no character 
      if (idx > 0 && !character.empty()) {

        int nwords = word_count(line, idx);

        // add character if doesn't exist, otherwise increment count
		//find returns an iterator - called it
		auto it = wcounts.find(character);
		if (it == wcounts.end()) {
			// not found, so add it
			
			wcounts.insert({ character, nwords});
		}
		else {
			// found, so add the nwords to total word count
			it->second += nwords;
		}

      } else {
        character = "";  // reset character
      }

    }
    file.close();  // close file
  }

}

void count_character_words_threadsafe(const std::string& filename, std::mutex& mutex, std::map<std::string, int>& wcounts) {

	std::string line;  // for storing each line read from the file
	std::ifstream file(filename);

	// read contents of file if open
	if (file.is_open()) {

		std::string character = "";  // empty character to start

									 // line by line
		while (std::getline(file, line)) {

			int idx = is_dialogue_line(line, character);

			//index will be -1 if the line is not a dialogue line, and character will be empty if there is no character 
			if (idx > 0 && !character.empty()) {

				int nwords = word_count(line, idx);

				// add character if doesn't exist, otherwise increment count
				//find returns an iterator - called it
				auto it = wcounts.find(character);
				if (it == wcounts.end()) {
					// not found, so add it
					while (!mutex.try_lock()) {}
						wcounts.insert({ character, nwords });
				}
				else {
					// found, so add the nwords to total word count
					while (!mutex.try_lock()) {}
					it->second += nwords;
				}
				mutex.unlock();

			}
			else {
				character = "";  // reset character
			}

		}
		file.close();  // close file
	}

}


/**
 * Comparator, orders by number decreasing, then by name
 * @param p1 first pair
 * @param p2 second pair
 * @return true if p1 should come before p2, false otherwise
 */
bool wc_greater_than(std::pair<std::string,int>& p1, std::pair<std::string,int>& p2) {

	//if p1 speaks more, return true, signifying p1
	if (p1.second > p2.second)
		return true;

	//if p1 and p2 speak the same amount, then return true (signifying p1 should come first)
	// if p1's name comes first in the alphabet
	if (p1.second == p2.second)
		if (p1.first < p2.first)
			return true;

  return false;
};

/**
 * Sorts characters in descending order by word count
 *
 * @param wcounts a map of character -> word count
 * @return sorted vector of {character, word count} pairs
 */
std::vector<std::pair<std::string,int>> sort_characters_by_wordcount(
    const std::map<std::string,int>& wcounts) {

  std::vector<std::pair<std::string,int>> out;
  out.reserve(wcounts.size());   // reserve memory for efficiency

  // sort characters by words descending
  for (const auto& pair : wcounts) {
    out.push_back(pair);
  }
  std::sort(out.begin(), out.end(), wc_greater_than);

  return out;
}

int main() {

  // map and mutex for thread safety
  std::mutex mutex;
  std::map<std::string,int> wcounts;

  std::vector<std::string> filenames = {
      "data/shakespeare_antony_cleopatra.txt",
      "data/shakespeare_hamlet.txt",
      "data/shakespeare_julius_caesar.txt",
      "data/shakespeare_king_lear.txt",
      "data/shakespeare_macbeth.txt",
      "data/shakespeare_merchant_of_venice.txt",
      "data/shakespeare_midsummer_nights_dream.txt",
      "data/shakespeare_much_ado.txt",
      "data/shakespeare_othello.txt",
      "data/shakespeare_romeo_and_juliet.txt",
  };

  //=============================================================
  // YOUR IMPLEMENTATION HERE TO COUNT WORDS IN MULTIPLE THREADS
  //=============================================================
  std::vector<std::thread> threads;

  

  auto t1 = std::chrono::high_resolution_clock::now(); //sets start time

													   //concurrent implementation 
	  for (int i = 0; i < filenames.size(); i++) {
		  count_character_words(filenames[i], mutex, wcounts);
	  }

  auto t2 = std::chrono::high_resolution_clock::now(); //sets end time
  long duration_single = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); //converts run time to ms

  
  
  t1 = std::chrono::high_resolution_clock::now(); //sets start time

	  for (int i = 0; i < filenames.size(); i++) {
		  
		  threads.push_back(std::thread( count_character_words_threadsafe , std::ref(filenames[i]), std::ref(mutex), std::ref(wcounts) ));
	  };
	  

	  for (int i = 0; i < filenames.size(); i++) {
		  threads[i].join();
	  }

  t2 = std::chrono::high_resolution_clock::now(); //sets end time
  long duration_multi = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); //converts run time to ms

 // std::cout << "time single: " << 0 << "\ttime multi: " << duration_multi << std::endl;




  auto sorted_wcounts = sort_characters_by_wordcount(wcounts);

  // results
  for (const auto& entry : sorted_wcounts) {
    std::cout << entry.first << "\t\t" << entry.second << std::endl;
  }

  std::cout << "time single: " << duration_single << "\ttime multi: " << duration_multi << std::endl;


  std::cout << std::endl << "Press ENTER to continue..." << std::endl;
  std::cin.get();

  return 0;
}
