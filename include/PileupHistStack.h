#ifndef PILEUP_HISTSTACK_H_
#define PILEUP_HISTSTACK_H_

#include "HistStack.h"

#include <string>
#include <vector>

class TFile;

class PileupHistStack : public HistStack {
 public:
  PileupHistStack(TFile* file, const std::string& path, const std::vector<std::string>& titles);

  void printTable();
};

#endif  // HISTSTACK_H_
