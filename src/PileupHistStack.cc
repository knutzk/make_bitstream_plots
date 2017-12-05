#include "PileupHistStack.h"
#include "HistCleaner.h"

#include "TFile.h"
#include "TProfile.h"

#include <iomanip>
#include <iostream>
#include <sstream>

PileupHistStack::PileupHistStack(TFile* file,
                                 const std::string& path,
                                 const std::vector<std::string>& titles)
    : HistStack(titles) {
  // Open the luminosity vs. pileup plot
  std::unique_ptr<TH1D> pileup{static_cast<TH1D*>(openCleanProfile(file, path + "Hits/Interactions_vs_lumi"))};
  if (!pileup) throw std::invalid_argument("Histogram not found");

  for (const auto& title : titles_) {
    std::unique_ptr<TH1D> hist{static_cast<TH1D*>(openCleanProfile(file, path + title))};
    if (!hist) throw std::invalid_argument("Histogram not found");

    // Create a profile that we fill with pileup/bandwidth usage pairs
    auto name = std::string("neu_") + hist->GetName();
    auto title_inc_axes = hist->GetTitle() + std::string(";pile-up;bitstream occ./module");
    auto pileup_hist = std::make_unique<TProfile>(TProfile{name.c_str(), title_inc_axes.c_str(), 16, 2.5, 82.5, "s"});
    pileup_hist->Approximate(kTRUE);

    for (unsigned int i = 1; i < pileup->GetNbinsX() + 1; ++i) {
      auto pu = pileup->GetBinContent(i);
      auto occ = hist->GetBinContent(i);
      pileup_hist->Fill(pu, occ);
    }

    // Aaand convert it into a TH1D
    std::unique_ptr<TH1D> pileup_hist_casted{dynamic_cast<TProfile*>(pileup_hist->Clone())->ProjectionX()};

    for (unsigned int i = 1; i < pileup_hist_casted->GetNbinsX() + 1; ++i) {
      auto error = pileup_hist_casted->GetBinError(i);
      pileup_hist_casted->SetBinError(i, error * 3);
    }
    pileup_hist_casted->SetName(pileup_hist->GetName());
    histograms_.emplace_back(std::move(pileup_hist_casted));
  }

  this->init();
}

std::string PileupHistStack::printTable() {
  // This prints the heads of the columns.
  std::ostringstream print;
  print << "pile-up";
  for (const auto& title : titles_short_) {
    print << "\t" << title;
  }
  print << std::endl;

  // Retrieve values from histograms with range [25, 75].
  for (unsigned int pileup = 25; pileup <= 75; pileup += 5) {
    print << pileup;
    for (const auto& hist : histograms_) {
      auto bin = hist->GetXaxis()->FindBin(pileup);
      if (hist->GetBinContent(bin) == 0) break;
      print << std::fixed << std::setprecision(1);
      print << "\t" << 100 * hist->GetBinContent(bin);
      print << " ± " << 100 * hist->GetBinError(bin) + 0.04999;
    }
    print << std::endl;
  }
  return print.str();
}

