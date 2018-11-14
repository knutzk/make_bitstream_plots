#ifndef DIRECTORYPARSER_H_
#define DIRECTORYPARSER_H_

#include <set>
#include <string>

class TFile;

/**
 * A class to parse a directory within a TFile object for
 * TProfile histograms. Effectively, this parser expect to find
 * to more subdirectory levels first, before taking any
 * histograms into account (first directory for the
 * layer/component, second for the stave/substructure, and then
 * the histograms for each module). The strings of all found
 * modules are stored in a set.
 */
class DirectoryParser {
public:
  /**
   * Main constructor to perform the parsing: this skims through
   * the given path in a TFile object and stores all histogram
   * names in the internal set. An additional wildcard can be
   * given to filter certain modules/components.
   * @param file The TFile object to be parsed
   * @param path The path within the TFile to be parsed
   * @param wildcard An additional wildcard to filter results
   */
  DirectoryParser(TFile* file, const std::string& path, const std::string& wildcard = "");

  /// Set of all module names found during parsing.
  std::set<std::string> modules;
};

#endif  // DIRECTORYPARSER_H_
