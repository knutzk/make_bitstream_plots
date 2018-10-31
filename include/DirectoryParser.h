#ifndef DIRECTORYPARSER_H_
#define DIRECTORYPARSER_H_

#include <set>
#include <string>

class TFile;

class DirectoryParser {
public:
  DirectoryParser(TFile* file, const std::string& path, const std::string& wildcard = "");

  std::set<std::string> modules;
};

#endif  // DIRECTORYPARSER_H_
