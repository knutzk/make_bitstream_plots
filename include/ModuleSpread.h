#ifndef MODULESPREAD_H_
#define MODULESPREAD_H_

#include <memory>
#include <string>
#include <vector>

class TFile;
class TH1D;


class ModuleSpread {
public:
  ModuleSpread(TFile* file, const std::string& path);

  std::vector<std::unique_ptr<TH1D>> getHistograms(const std::string& filter);
  std::vector<std::unique_ptr<TH1D>> getEtaSlices(float pu_des = 75);

private:
  TFile* file_;
  std::vector<std::string> modules_;
  const std::string path_;
};

#endif  // MODULESPREAD_H_
