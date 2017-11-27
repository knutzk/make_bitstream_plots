#include "PileupHistStack.h"
#include "HistCleaner.h"

#include "TFile.h"
#include "TProfile.h"

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
    auto pileup_hist = std::make_unique<TProfile>(TProfile{name.c_str(), title_inc_axes.c_str(), 18, 2.5, 92.5});
    pileup_hist->Approximate(kTRUE);

    for (unsigned int i = 1; i < pileup->GetNbinsX() + 1; ++i) {
      auto pu = pileup->GetBinContent(i);
      auto occ = hist->GetBinContent(i);
      pileup_hist->Fill(pu, occ);
    }

    // Aaand convert it into a TH1D
    std::unique_ptr<TH1D> pileup_hist_casted{dynamic_cast<TProfile*>(pileup_hist->Clone())->ProjectionX()};
    pileup_hist_casted->SetName(pileup_hist->GetName());
    histograms_.emplace_back(std::move(pileup_hist_casted));
  }

  this->init();
}

void PileupHistStack::printTable() {
  // This prints the heads of the columns.
  std::cout << "pile-up\t";
  for (const auto& title : titles_short_) {
    std::cout << title << "\t\t";
  }
  std::cout << std::endl;

  // Retrieve values from histograms with range [30, 80].
  for (unsigned int pileup = 30; pileup <= 80; pileup += 5) {
    std::cout << pileup << "\t";
    for (const auto& hist : histograms_) {
      auto bin = hist->GetXaxis()->FindBin(pileup);
      if (hist->GetBinContent(bin) == 0) break;
      std::cout << std::fixed << std::setprecision(1);
      std::cout << 100 * hist->GetBinContent(bin);
      std::cout << " +/- " << 100 * hist->GetBinError(bin) + 0.04999;
      std::cout << "\t";
    }
    std::cout << std::endl;
  }
}

