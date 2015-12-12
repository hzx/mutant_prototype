#pragma once
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
string getRealPath(string const& path);
int findSymbol(string const& content, char symbol, int left, int right);
void splitNames(vector<string>& names, string& content, int left, int right);
string getExtension(const string& filename);
string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
string base64Encode(const string& value);
