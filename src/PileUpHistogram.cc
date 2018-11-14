#include "PileUpHistogram.h"

#include "TProfile.h"
#include "TH1D.h"
#include "TFile.h"

#include <set>
#include <string>

PileUpHistogram::PileUpHistogram(TFile* file, const std::string& path, const std::string& histo_name)
  : m_file(file)
  , m_path(path)
  , m_histo_name(histo_name)
{
  // do nothing for now
}

void PileUpHistogram::fillHisto() {
  std::string full_path = m_path + "Errors/Modules_BitStr_Occ_Tot/" + m_histo_name;
  auto prof = static_cast<TProfile*>(m_file->Get(full_path.c_str()));
  if (!prof) throw std::invalid_argument{"Histogram " + full_path + " not found"};

  // Clean all vetoed luminosity blocks.
  for (const auto& lb : m_vetoed_lbs) {
    prof->SetBinContent(lb, 0);
    prof->SetBinEntries(lb, 0);
    prof->SetBinError(lb, 0);
  }

  std::unique_ptr<TH1D> hist{static_cast<TH1D*>(prof)};

  // Create a profile that we fill with pileup/bandwidth usage pairs
  auto name = std::string(std::tmpnam(nullptr)) + hist->GetName();
  auto title_inc_axes = hist->GetTitle() + std::string(";pile-up;bandwidth usage");
  auto tmp_hist = std::make_unique<TProfile>(TProfile{name.c_str(), title_inc_axes.c_str(), m_pile_up_bins, m_pile_up_min, m_pile_up_max, "s"});
  tmp_hist->Approximate(kTRUE);

  // Open the luminosity vs. pileup plot
  std::unique_ptr<TH1D> pileup{static_cast<TH1D*>(m_file->Get((m_path + "Hits/Interactions_vs_lumi").c_str()))};
  if (!pileup) throw std::invalid_argument("Pile-up histogram not found");

  for (unsigned int i = 1; i < pileup->GetNbinsX() + 1; ++i) {
    auto pu = pileup->GetBinContent(i);
    auto occ = hist->GetBinContent(i);
    if (occ == 0.) continue;
    tmp_hist->Fill(pu, occ);
  }

  // And finally save everything
  m_histo.reset(dynamic_cast<TProfile*>(tmp_hist->Clone())->ProjectionX());
  m_histo->SetName(tmp_hist->GetName());
}

void PileUpHistogram::setPileUpRange(float min, float max) {
  if (min < 0 || min >= max) throw std::invalid_argument("Check pile-up range for histograms");
  int n_bins = std::floor((max - min)/5);
  m_pile_up_min = min;
  m_pile_up_max = max;
  m_pile_up_bins = n_bins;
}

void PileUpHistogram::vetoLumiBlocks(const std::set<int> lbs) {
  for (const auto& lb : lbs) {
    m_vetoed_lbs.insert(lb);
  }
}
