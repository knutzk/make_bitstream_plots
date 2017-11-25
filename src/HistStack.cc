#include "HistStack.h"
#include "HistCleaner.h"

#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TProfile.h"

#include <iostream>

HistStack::HistStack(std::vector<TH1D*> hists)
  : histograms_(hists) {
  for (const auto& hist : hists) {
    titles_.push_back(hist->GetName());
  }

  this->init();
}

HistStack::HistStack(TFile* file, const std::string& path, const std::vector<std::string>& titles, double x_max)
  : titles_(titles) {
  for (const auto& title : titles_) {
    auto hist = static_cast<TH1D*>(openCleanProfile(file, path + title));
    histograms_.emplace_back(hist);
  }

  this->init(x_max);
}

void HistStack::createLegend(TLegend* legend) {
  auto get_suffix = [](const std::string& s) {
    auto pos = s.rfind("_");
    return s.substr(pos + 1, s.length() - pos);
  };

  auto substitute_B_with_L = [](std::string* s) {
    auto pos = s->find("B");
    if (pos != std::string::npos) s->replace(pos, 1, std::string("L"));
  };

  for (const auto& hist : histograms_) {
    auto name = get_suffix(std::string(hist->GetName()));
    substitute_B_with_L(&name);
    titles_short_.push_back(name);
    legend->AddEntry(hist, name.c_str());
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
    if (hist->GetNbinsX() > 1000) hist->Rebin(100);
    if (x_max != 0) hist->GetXaxis()->SetRangeUser(0, x_max);
    hist->SetMarkerColor(color_index);
    hist->SetLineColor(color_index);
    hist->SetMarkerStyle(19 + marker_index);
  }
}

void HistStack::setComfortableMax(double max) {
  has_custom_max_ = true;
  max *= 1.4;
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
