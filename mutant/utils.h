#ifndef MUTANT_UTILS_H
#define MUTANT_UTILS_H


#include <string>
#include <vector>


using std::string;
using std::vector;


vector<string> parseGeneric(string const& src, const char delim);
string joinPath(string& dir, const char* name);
bool existsPath(string& path);
string getFileContent(string const& path);
string getCwd();


#endif /* end of include guard: MUTANT_UTILS_H */
