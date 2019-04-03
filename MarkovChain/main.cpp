#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <random>
#include <sstream>
using namespace std;

class MarkovChain {
public:
	MarkovChain(const string& file_name_ = "") { 
#ifdef _DEBUG
		input_words = { "now", "he", "is", "gone", "she", "said",
						"he", "is", "gone", "for", "good" };
#else
		LoadFromFile(file_name_);
#endif // _DEBUG
	}

	void Generate(int output_length_, int prefix_length_ = 2) {
		map<vector<string>/*PREFIX*/, vector<string>/*POSTFIX*/> dict;

		for (unsigned i = 0; i < input_words.size() - prefix_length_; i++) {
			vector<string> prefix(prefix_length_);
			string postfix;

			// Fill in the prefix with prefix_length_ words from input_words
			int j;
			for (j = 0; j < prefix_length_ && i + j < input_words.size() - 1; j++)
				prefix[j] = input_words[i + j];
			postfix = input_words[i + j];

			dict[prefix].push_back(postfix);
		}

		random_device device;
		mt19937 generator(device());

		// Selecting random element from dictionary
		auto it = dict.begin();
		uniform_int_distribution<> rand_element(0, dict.size() - 1);
		advance(it, rand_element(generator));
		pair<vector<string>, vector<string>> element = *it;

		// Pushing random prefix right to the output word vector
		output_words.insert(output_words.end(), element.first.begin(), element.first.end());

		for (int i = 0; i < output_length_ && !element.second.empty(); i++) {
			// Distribution of integers meaning position in postfix vector
			uniform_int_distribution<> rand_postfix(0, element.second.size() - 1);

			// Retrieving data from element
			vector<string> prefix = element.first;
			string postfix = element.second[rand_postfix(generator)];

			// Pushing only postfix 
			output_words.push_back(postfix);

			// Formating new prefix from the prev. one after the first word, adding postfix
			prefix = vector<string>(prefix.begin() + 1, prefix.end());
			prefix.push_back(postfix);

			element = make_pair(prefix, dict[prefix]);
		}
	}

	void Present() {
		for (auto word : output_words)
			cout << word << " ";
		cout << "\n";
	}

	void Reset() {
		input_words.clear();
		output_words.clear();
	}

	void LoadFromFile(const string& file_name_) {
		fstream input_file(file_name_, ios::in);
		if (!input_file.is_open()) return;

		string word;
		while (input_file >> word)
			input_words.push_back(word);

		input_file.close();
	}

	vector<string> GetRawResult() {
		return output_words;
	}

	string GetFormatedResult() {
		string formated_result;
		for (auto word : output_words)
			formated_result += word + " ";
		return formated_result;
	}

private:
	vector<string> input_words;
	vector<string> output_words;
};

int main(int argc, char** argv) {
	// Very simple argument parsing. 
	// The order of arguments is: [file name] [result word count] [depth]

	string file_name;
	if (argc > 1)
		file_name = string(argv[1]);

	int result_size = 300;
	if (argc > 2) {
		stringstream sst;
		sst << string(argv[2]);
		sst >> result_size;
	}

	int depth = 2;
	if (argc > 3) {
		stringstream sst;
		sst << string(argv[3]);
		sst >> depth;
	}
		
	MarkovChain mc(file_name);
	mc.Generate(result_size, depth);
	mc.Present();

	return 0;
}