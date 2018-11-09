#include "HistStack.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TProfile.h"

#include <regex>
#include <iostream>
#include <iomanip>

HistStack::HistStack(std::vector<std::unique_ptr<TH1D> >& histos, double x_max) {
  for (auto& hist : histos) {
    auto ptr = hist.release();
    histograms_.emplace_back(ptr);
  }

  this->init(x_max);
}

void HistStack::createLegend(TLegend* legend) {
  for (const auto& hist : histograms_) {
    auto name = std::string(hist->GetName());
    titles_short_.push_back(name);
    legend->AddEntry(hist.get(), name.c_str(), "P");
  }
}

void HistStack::draw(TCanvas* canvas) {
  if (!has_custom_max_) this->setComfortableMax(this->getMax());
  for (const auto& hist : histograms_) {
    if (hist == histograms_.front()) {
      hist->Draw("E");
    } else {
      hist->Draw("PE SAME");
    }
  }
}

double HistStack::getMax() {
  double max{0.};
  for (const auto& hist : histograms_) {
    for (unsigned int i = 1; i < hist->GetNbinsX() + 1; ++i) {
      if (hist->GetBinContent(i) > max) {
        max = hist->GetBinContent(i);
      }
    }
  }
  return max;
}

void HistStack::init(double x_max) {
  int marker_index = 0;
  int color_index = 0;

  for (auto& hist : histograms_) {
    color_index++;
    marker_index++;
    if (color_index == 5) color_index++;
    if (marker_index == 3) marker_index++;
    if (hist->GetNbinsX() > 1000) hist->Rebin(50);
    if (x_max != 0) hist->GetXaxis()->SetRangeUser(0, x_max);
    hist->SetMarkerColor(color_index);
    hist->SetLineColor(color_index);
    hist->SetMarkerStyle(19 + marker_index);
  }
}

std::string HistStack::printTable() {
  // This prints the heads of the columns.
  std::ostringstream print;
  print << "pile-up";
  for (const auto& title : titles_short_) {
    print << "\t" << title;
  }
  print << std::endl;

  // Retrieve values from histograms with range [25, 75].
  for (int i = 1; i <= histograms_.at(0)->GetNbinsX(); ++i) {
    auto pileup = histograms_.at(0)->GetBinCenter(i);
    print << pileup;
    for (const auto& hist : histograms_) {
      if (hist->GetBinContent(i) == 0) break;
      print << std::fixed << std::setprecision(1);
      print << "\t" << 100 * hist->GetBinContent(i);
      print << " ± " << 100 * hist->GetBinError(i) + 0.04999;
    }
      print << std::endl;
  }
  return print.str();
}

void HistStack::setComfortableMax(double max) {
  has_custom_max_ = true;
  max *= 1.3;
  unsigned int order = 0;
  while (max < 1) {
    max *= 10;
    order++;
  }
  max = floor(max + 1) / pow(10, order);
  for (auto& hist : histograms_) {
    hist->GetYaxis()->SetRangeUser(0, max);
  }
}

void HistStack::setXAxisTitle(const std::string& title) {
  for (auto& hist : histograms_) {
    hist->GetXaxis()->SetTitle(title.c_str());
  }
}

void HistStack::setXAxisTicks(unsigned int ticks) {
  for (auto& hist : histograms_) {
    hist->GetXaxis()->SetNdivisions(ticks, kTRUE);
  }
}

void HistStack::setYAxisTitle(const std::string& title) {
  for (auto& hist : histograms_) {
    hist->GetYaxis()->SetTitle(title.c_str());
  }
}

void HistStack::shift(const std::vector<float>& shift_values) {
  if (shift_values.size() != histograms_.size()) {
    throw std::invalid_argument("Given shift-value vector must have the same number of entries as histograms");
  }

  auto min = histograms_.at(0)->GetXaxis()->GetXmin();
  auto max = histograms_.at(0)->GetXaxis()->GetXmax();
  auto width = histograms_.at(0)->GetBinWidth(1);
  int counter{-1};
  for (auto& hist : histograms_) {
    counter++;
    auto shift = shift_values.at(counter) * width;
    std::cout << "Shifting histogram " << hist->GetName() << " by " << shift << std::endl;
    hist->GetXaxis()->SetLimits(min + shift, max + shift);
  }
}
