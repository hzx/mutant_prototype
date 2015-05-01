#ifndef MUTANT_UTILS_H
#define MUTANT_UTILS_H


#include <string>
#include <vector>


using std::string;
using std::vector;


vector<string> parseGeneric(string const& src, const char delim);
string joinPath(string& dir, char const* name);
bool existsFile(string& filename);
bool existsDir(string& path);
string getFileContent(string const& path);
string getCurrentDir();
int findSymbol(string const& content, char symbol, int left, int right);
void splitNames(vector<string>& names, string& content, int left, int right);


#endif /* end of include guard: MUTANT_UTILS_H */
