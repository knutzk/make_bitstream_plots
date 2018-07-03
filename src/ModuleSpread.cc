#include "ModuleSpread.h"
#include "HistCleaner.h"
#include "TFile.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TKey.h"
#include "TClass.h"
#include "TCanvas.h"
#include "TTree.h"

#include <iostream>
#include <regex>

ModuleSpread::ModuleSpread(TFile* file, const std::string& path)
  : file_(file)
  , path_(path) {
  auto dir = file_->GetDirectory(std::string(path_ + "Errors/Modules_BitStr_Occ_Tot/").c_str());

  TIter nextkey(dir->GetListOfKeys());
  TKey *key = nullptr;
  while ((key = static_cast<TKey*>(nextkey()))) {
    TObject *obj = key->ReadObj();
    if (!obj->IsA()->InheritsFrom(TDirectory::Class())) continue;

    TIter nextkey2(dir->GetDirectory(obj->GetName())->GetListOfKeys());
    TKey *key2 = nullptr;
    while ((key2 = static_cast<TKey*>(nextkey2()))) {
      TObject *obj2 = key2->ReadObj();
      if (!obj2->IsA()->InheritsFrom(TDirectory::Class())) continue;

      TIter nextkey3(dir->GetDirectory(std::string(std::string(obj->GetName()) + "/" + obj2->GetName()).c_str())->GetListOfKeys());
      TKey *key3 = nullptr;
      while ((key3 = static_cast<TKey*>(nextkey3()))) {
        TObject *obj3 = key3->ReadObj();
        if (!obj3->IsA()->InheritsFrom(TProfile::Class())) continue;

        modules_.push_back(std::string(dir->GetPath()) + "/" + obj->GetName() + "/" + obj2->GetName() + "/" + obj3->GetName());
      }
    }
  }

  for (auto&& it = modules_.begin(); it != modules_.end(); ++it) {
    it->replace(0, std::string(file_->GetName()).size() + 2, "");
  }
}

std::vector<std::unique_ptr<TH1D>> ModuleSpread::getHistograms(const std::string& filter) {
  // Open the luminosity vs. pileup plot
  std::unique_ptr<TH1D> pileup{static_cast<TH1D*>(openCleanProfile(file_, path_ + "Hits/Interactions_vs_lumi"))};
  if (!pileup) throw std::invalid_argument("Histogram not found");

  std::vector<std::unique_ptr<TH1D>> histograms;
  histograms.emplace_back(std::make_unique<TH1D>(std::string("pu75_" + filter).c_str(), "pu75;Bandwidth usage/module;# modules", 50, 0.0, 1.0));
  histograms.emplace_back(std::make_unique<TH1D>(std::string("pu70_" + filter).c_str(), "pu70", 50, 0.0, 1.0));

  int spread_counter{0};
  int spread2_counter{0};
  int module_counter{0};

  for (const auto& m : modules_) {
    std::smatch match;
    std::regex_search(m, match, std::regex(filter));
    if (match.empty()) continue;
    module_counter++;
    std::unique_ptr<TH1D> hist{static_cast<TH1D*>(openCleanProfile(file_, m))};
    if (!hist) throw std::invalid_argument("Histogram not found");

    for (unsigned int i = 1; i < pileup->GetNbinsX() + 1; ++i) {
      auto pu = pileup->GetBinContent(i);
      if (std::abs(pu - 75) < 2.5) {
        histograms.at(0)->Fill(hist->GetBinContent(i));
        spread_counter++;
      } else if (std::abs(pu - 70) < 2.5) {
        histograms.at(1)->Fill(hist->GetBinContent(i));
        spread2_counter++;
      }
    }
  }

  histograms.at(1)->SetLineColor(kRed);
  histograms.at(0)->Scale(1./spread_counter * module_counter);
  histograms.at(1)->Scale(1./spread2_counter * module_counter);

  return histograms;
}

std::vector<std::unique_ptr<TH1D>> ModuleSpread::getEtaSlices(float pu_des) {
  // Open the luminosity vs. pileup plot
  std::unique_ptr<TH1D> pileup{static_cast<TH1D*>(openCleanProfile(file_, path_ + "Hits/Interactions_vs_lumi"))};
  if (!pileup) throw std::invalid_argument("Histogram not found");

  std::vector<std::unique_ptr<TH1D>> histograms;
  histograms.emplace_back(std::make_unique<TH1D>("m0", "m0;Bandwidth usage/module;# modules", 12, 0.4, 1.0));
  histograms.emplace_back(std::make_unique<TH1D>("m1", "m1", 12, 0.4, 1.0));
  histograms.emplace_back(std::make_unique<TH1D>("m2", "m2", 12, 0.4, 1.0));
  histograms.emplace_back(std::make_unique<TH1D>("m3", "m3", 12, 0.4, 1.0));
  histograms.emplace_back(std::make_unique<TH1D>("m4", "m4", 12, 0.4, 1.0));
  histograms.emplace_back(std::make_unique<TH1D>("m5", "m5", 12, 0.4, 1.0));
  histograms.emplace_back(std::make_unique<TH1D>("m6", "m6", 12, 0.4, 1.0));

  int counter[7] = {0};

  for (const auto& m : modules_) {
    std::unique_ptr<TH1D> hist{static_cast<TH1D*>(openCleanProfile(file_, m))};
    if (!hist) throw std::invalid_argument("Histogram not found");

    std::smatch match;
    std::regex_search(m, match, std::regex("_M[0-9][A|C]?_"));
    if (match.empty()) throw std::invalid_argument("Could not find module ID");
    auto module_eta = std::stoi(match.str().substr(2, match.str().size() - 3));

    for (unsigned int i = 1; i < pileup->GetNbinsX() + 1; ++i) {
      if (std::abs(pileup->GetBinContent(i) - pu_des) > 2.5) continue;
      histograms.at(module_eta)->Fill(hist->GetBinContent(i));
      counter[module_eta]++;
    }
  }

  int color_index{0};
  for (const auto& h : histograms) {
    h->Scale(1./counter[color_index]);
    h->SetLineColor(color_index);
    color_index++;
  }

  return histograms;
}
