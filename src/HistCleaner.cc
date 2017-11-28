#include "TFile.h"
#include "TProfile.h"

TProfile* openCleanProfile(TFile* file, const std::string& name) {
  auto hist = static_cast<TProfile*>(file->Get(name.c_str()));
  if (!hist) {
    throw std::invalid_argument{"Histogram " + name + " could not be found"};
  }
  for (unsigned int i = 0; i < 124; ++i) {
    hist->SetBinContent(i, 0);
    hist->SetBinEntries(i, 0);
    hist->SetBinError(i, 0);
  }
  return hist;
}
