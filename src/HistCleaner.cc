#include "TProfile.h"
#include "TFile.h"

TProfile* openCleanProfile(TFile* file, const std::string& name) {
  auto hist = static_cast<TProfile*>(file->Get(name.c_str()));
  for (unsigned int i = 0; i < 201; ++i) {
    hist->SetBinContent(i, 0);
    hist->SetBinEntries(i, 0);
    hist->SetBinError(i, 0);
  }
  return hist;
}
