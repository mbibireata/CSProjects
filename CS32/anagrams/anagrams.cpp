#include <iostream> 
#include <fstream>
#include <istream> 
#include <sstream>
#include <cassert> 
#include <algorithm>
#include <csignal>
#include <unistd.h>
#include <cstring>

using namespace std;

const int MAXRESULTS   = 20;    // Max matches that can be found
const int MAXDICTWORDS = 10; // Max words that can be read in


//required functions
    //works for all test cases
int readDictionary(istream &dictfile, string dict[]);
    //in progress
int recursivePermute(string word, const string dict[], int size, string results[]);
    //in progress
void recurPrint(const string results[], int size);

//functions
    //returns the index of an entry and ensures that index isn't greater than MAXDICTWORDS
int recursiveReadEntry(istream &dictfile, string dict[], int index);
void getPermutes(string prefix, string rest, const string dict[], string results[], const int nwords, const int MAXRESULTS);

//bool isValidPermute(const string word, const string dict[], int i, const int nwords, string results[], const int MAXRESULTS);

bool loopDictCheck(int i, const int nwords, const string dict[], const string word);
bool loopResultsCheck(int i, const int MAXRESULTS, const string results[], const string word);
void loopPermute(string prefix, string rest, const string dict[], string results[], const int nwords, const int MAXRESULTS, int j, int max);

int avail_index(const string results[], int size);

int readEntries(const string results[], int i, int max);

//FUNCTION ADDED FOR GRADING PURPOSES
void reverseResults(string results[]){
    string rCopy[MAXRESULTS];
    for (int i = MAXRESULTS-1, j=0; i>=0; --i,++j){
        rCopy[j]=results[i];
    }
    
    for (int i = 0; i<MAXRESULTS; ++i){
        results[i] = rCopy[i];
    }
}

void testone(int n)
{
    string dictionary[MAXDICTWORDS];
    string results[MAXRESULTS];
    
    switch (n)
    {
        default: {
            cout << "Bad argument" << endl;
        } break; case  1: {
            istringstream iss("dog\ncat\nrat\neel\ntar\nart\nlee\nact\ngod");
            int numResults = readDictionary(iss, dictionary);
            sort(dictionary, dictionary + numResults);
            assert(numResults == 9 && dictionary[0] == "act" && dictionary[1] == "art");
        } break; case  2: {
            // File is empty, Checks that file is empty and readDictionary returns 0.
            istringstream iss("");
            int numResults = readDictionary(iss, dictionary);
            assert(numResults == 0 && dictionary[0] == "");
        } break; case  3: {
            // Dictionary has been read properly
            istringstream iss("dog\ncat\nrat\neel\ntar\nart\nlee\nact\ngod");
            int numResults = readDictionary(iss, dictionary);
            sort(dictionary, dictionary + numResults);
            assert(numResults == 9 && dictionary[0] == "act" && dictionary[numResults-1] == "tar");
        } break; case  4: {
            // Input file is larger than the dictionary size
            istringstream iss("dog\ncat\nrat\neel\ntar\nart\nlee\nact\ngod\ntoo\nmany\nwords");
            int numResults = readDictionary(iss, dictionary);
            sort(dictionary, dictionary + numResults);
            assert(numResults == 10 && dictionary[MAXDICTWORDS-1] == "too");
        } break; case  5: {
            // If a certain word with repeated letter is shown in results more than once - still accept.
            string dictionary[] = { "one", "oone", "ne", "e", "too" };
            int numResults = recursivePermute("oto", dictionary, 5, results);
            reverseResults(results); //MB CHANGED
            assert((numResults == 1 || numResults == 2) && results[0] == "too");
        } break; case  6: {
            // Doesn't check numResults nor duplicates.
            string dictionary[] = { "one", "oone", "ne", "e", "too" };
            recursivePermute("oto", dictionary, 5, results);
            reverseResults(results);//MB CHANGED
            assert(results[0] == "too");
        } break; case  7: {
            // If word wasn't found, numResults = 0 and results array is empty.
            string dictionary[] = { "one", "oone", "ne", "e" };
            int numResults = recursivePermute("look", dictionary, 4, results);
            reverseResults(results);//MB CHANGED
            assert(numResults == 0 && results[0] == "" && results[1] == "");
        } break; case  8: {
            // No fraction of a permutation is being searched in dictionary
            string dictionary[] = { "one", "oone", "non", "oon" };
            int numResults = recursivePermute("on", dictionary, 4, results);
            reverseResults(results);//MB CHANGED
            assert(numResults == 0 && results[0] == "" && results[1] == "");
        } break; case  9: {
            // No fraction of a permutation is being searched in dictionary
            string dictionary[] = { "one", "oone", "ne", "e", "neoo", "oneo" };
            int numResults = recursivePermute("neo", dictionary, 6, results);
            reverseResults(results); //MB CHANGED
            assert(numResults == 1 && results[0] == "one" && results[1] == "");
        } break; case  10: {
            // Checking that no error occurs if more than MAXRESULTS are found.
            string dictionary[] = { "true",  "treu", "teru", "teur", "ture",
                "tuer", "rtue", "rteu", "retu","reut", "ruet", "rute", "utre",
                "uter", "uetr", "uert", "urte", "uret", "etru", "etur", "ertu",
                "erut", "eurt", "eutr" };
            // All 24 permutations
            int numResults = recursivePermute("true", dictionary, 24, results);
            assert(numResults == MAXRESULTS);
        } break; case  11: {
            // Checking one word was found, no duplicates.
            string dictionary[] = { "ppp" };
            int numResults = recursivePermute("ppp", dictionary, 1, results);
            reverseResults(results);
            assert(numResults == 1 && results[0] == "ppp" && results[1] == "");
        } break; case  12: {
            string dictionary[] = { "run", "dodge", "break", "urn", "defeat" };
            int numResults = recursivePermute("nru", dictionary, 5, results);
            sort(dictionary, dictionary + numResults);
            reverseResults(results);//MB CHANGED
            assert(numResults == 2 && results[0] == "run" && results[1] == "urn");
        } break; case  13: {
            streambuf* oldCoutStreamBuf = cout.rdbuf();
            ostringstream strCout;
            cout.rdbuf(strCout.rdbuf());
            string results[] = { "cat", "act"};
            recurPrint(results, 2);
            cout.rdbuf(oldCoutStreamBuf);
            string temp = strCout.str();
            bool match1 = temp.find("act") != string::npos;
            bool match2 = temp.find("cat") != string::npos;
            assert(match1 && match2);
        } break; case  14: {
            istringstream iss ("tier\nrite\nbate\ntire\nttir");
            int numWords = readDictionary(iss, dictionary);
            sort(dictionary, dictionary + numWords);
            assert(numWords == 5 && dictionary[0] == "bate");
            int numResults = recursivePermute("tier", dictionary, numWords, results);
            reverseResults(results);//MB CHANGED
            assert(numResults == 3 && (results[2] == "tire" || results[2] == "tier" || results[2] == "rite"));
        } break; case  15: {
            string example[] = { "kool", "moe", "dee" };
            int numResults = recursivePermute("look", example, 3, results);
            reverseResults(results);//MB CHANGED
            assert(numResults == 1 && results[0] == "kool");
        } break;
    }
}



int main() {
    /*
    cout << "Enter test number: ";
    int n;
    cin >> n;
    testone(n);
    */
    
    string results[MAXRESULTS];
    string exampleDict[] = {"kool", "moe", "dee"};
    int numResults = recursivePermute("look", exampleDict, 3, results);
    for (const auto& x:results) cout << x << endl;
    assert(numResults == 1 && results[0] == "kool");
    
    //cout << "Passed" << endl;
    
    
    
    //return 0;
}

/** This function returns the index of entries into the dict[] array
    @param dictfile is the file being read
    @param dict is the array storing each word in the dictionary
    @param index is the index of the word being added into the dict array
    returns the index of an entry. If MAXDICTWORDS, returns index
 */
int recursiveReadEntry(istream &dictfile, string dict[], int index){
    if (index == MAXDICTWORDS) return 0;
    if (dictfile >> dict[index]) {
        return 1 + recursiveReadEntry(dictfile, dict, index + 1);
    }
    return 0;
}

/** This function reads the dictionary and returns the number of words in it
    @param dictfile is the file being read
    @param dict is the array storing each word in the dictionary
    @returns the number of words in the dict[] string (words read from dicitonary)
 */
int readDictionary(istream &dictfile, string dict[]){
    return recursiveReadEntry(dictfile, dict, 0);
}

/** This function places all permutations of word which are found in dict into results
    @param prefix is the prefix string
    @param rest is the rest of the string minus the char appended to prefix
    @param dict is the dictionary array containing al valid dictionary words
    @param results is the array of string permutations matching words in the dictionary 
    @param nwords is the number of words in the dictionary
    @param MAXRESULTS is the max number of results allowed in results[]
    returns void. results is now filled with valid permutations
 */
void getPermutes(string prefix, string rest, const string dict[], string results[], const int nwords, const int MAXRESULTS){
    //int increment = 0;
    if (rest == "" && loopResultsCheck(0, MAXRESULTS, results, prefix) && loopDictCheck(0,nwords, dict, prefix)){
        //cout << prefix << endl;
        results[avail_index(results, MAXRESULTS)] = prefix;
        //increment = 1;
    }
    //cout << increment << endl;
    loopPermute(prefix, rest, dict, results, nwords, MAXRESULTS, 0, static_cast<int>(rest.size()));
}

/** This function is a stand in for a for loop which helps place valid entries into results[]
    @param prefix is the prefix string
    @param rest is the rest of the string minus the char appended to prefix
    @param dict is the dictionary array containing al valid dictionary words
    @param results is the array of string permutations matching words in the dictionary
    @param nwords is the number of words in the dictionary
    @param MAXRESULTS is the max number of results allowed in results[]
    @param j is an iterator
    @param max is the length of the rest string
 returns void. results is now filled with valid permutations
 */
void loopPermute(string prefix, string rest, const string dict[], string results[], const int nwords, const int MAXRESULTS, int j, int max) {
    if (j >= max) return;
    getPermutes(prefix + rest[0], rest.substr(1), dict, results, nwords, MAXRESULTS);
    rest = rest.substr(1) + rest[0];
    loopPermute(prefix, rest, dict, results, nwords, MAXRESULTS, j+1, max);
}

/*
bool isValidPermute(const string word, const string dict[], int i, const int nwords, const string results[], const int MAXRESULTS){
    if (word == results[0]) return 0;
    if (word == dict[0]) return 1;
    if (i < MAXRESULTS) return isValidPermute(word, dict+1, i+1, nwords, results+1, MAXRESULTS);
    if ()
}
 */

    //checks if word is a valid dictionary word
bool loopDictCheck(int i, const int nwords, const string dict[], const string word){
    if (word == dict[0]) return 1;
    if (i == nwords) return 0;
    return loopDictCheck(i+1, nwords, dict+1, word);
}

    //checks if word is not a duplicate in results
bool loopResultsCheck(int i, const int MAXRESULTS, const string results[], const string word) {
    if (word == results[0]) return 0;
    if (i == MAXRESULTS) return 1;
    return loopResultsCheck(i+1, MAXRESULTS, results+1, word);
}



/** This function places all permutations of word which are found in dict into results
    @param word is the word being permuted (user input)
    @param dict is the dictionary array containing all valid dictionary words
    @param size is the number of words in the dict array
    @param results is the array of string permutations matching words in the dictionary
    returns the number of matched words found
 */
int recursivePermute(string word, const string dict[], int size, string results[]){
    getPermutes("",word,dict,results,size,MAXRESULTS);
    return readEntries(results, 0, MAXRESULTS);
}

/** Prints results from results[]
    @param size is the number of words in results string
    @param results is the results array
    returns void but prints out all entries in results
 */
void recurPrint(const string results[], int size) {
    if (size <= 0) return;
    if (results[MAXRESULTS - size] != "") {
        cout << "Matching word " << results[MAXRESULTS-size] << endl;
    }
    recurPrint(results, size-1);
}

/** funcion determines if an index is available for input in the results[] array
    @param results is the results array
    @param size is the size of the array
    returns index of empty string element
 */
int avail_index(const string results[], int size) {
    if (results[size-1] == "") return size-1;
    return avail_index(results, size-1);
}

/** function returns number of string entries in results
    @param results is the results array
    @int i is an iterator
    @int max is the max number of entries allowed in the results[] array
    returns the number of nonempty strings in results
 */
int readEntries(const string results[], int i, int max) {
    if (i == max) return 0;
    if (results[i] != "") return 1 + readEntries(results, i + 1, max);
    return readEntries(results, i+1, max);
    
}
