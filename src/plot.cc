#include "HistStack.h"
#include "HistCleaner.h"
#include "AtlasStyle.h"
#include "AtlasLabels.h"

#include "TLatex.h"
#include "TProfile.h"
#include "TH1D.h"
#include "TFile.h"
#include "TLegend.h"
#include "TCanvas.h"

#include <memory>

void SupportLabel(double xpos, double ypos, const std::string& text);
std::unique_ptr<TProfile> getVersusPileup(TH1D* pileup_hist, TH1D* hist);

int main() {
  SetAtlasStyle();

  auto file = TFile::Open("file.root", "READ");
  std::string path = "run_339957/Pixel/Errors/";
  auto canvas = new TCanvas("canvas", "canvas", 600, 600);

  // =======================================================
  // Bit-stream occupancy by FE/MCC errors

  // We need more space on the left for these plots.
  canvas->SetLeftMargin(0.2);
  gStyle->SetTitleYOffset(1.8);

  std::vector<std::string> hist_titles;
  hist_titles.push_back("Bitstr_Occ_Errors_LB_B0");
  hist_titles.push_back("Bitstr_Occ_Errors_LB_B1");
  hist_titles.push_back("Bitstr_Occ_Errors_LB_B2");
  hist_titles.push_back("Bitstr_Occ_Errors_LB_ECA");
  hist_titles.push_back("Bitstr_Occ_Errors_LB_ECC");

  std::vector<std::unique_ptr<TProfile>> versus_pileup;
  for (const auto& title : hist_titles) {
    versus_pileup.push_back(getVersusPileup(static_cast<TH1D*>(openCleanProfile(file, "run_339957/Pixel/Hits/Interactions_vs_lumi")), static_cast<TH1D*>(openCleanProfile(file, path + title))));
  }

  std::vector<TH1D*> versus_pileup_projection;
  for (auto& hist : versus_pileup) {
    hist->Rebin(2);
    versus_pileup_projection.push_back(hist->ProjectionX());
    versus_pileup_projection.back()->SetName(hist->GetName());
  }

  auto legend = new TLegend(0.8, 0.6, 0.9, 0.9);
  legend->SetTextFont(42);
  legend->SetTextSize(0.05);
  std::unique_ptr<HistStack> stack = std::make_unique<HistStack>(HistStack{versus_pileup_projection});
  stack->setXAxisTitle("Average #mu per lumi block");
  stack->setYAxisTitle("Average bandwidth occupancy");
  stack->createLegend(legend);
  stack->draw(canvas);
  legend->Draw("SAME");
  ATLASLabel(0.24, 0.88, "Pixel Internal");
  SupportLabel(0.24, 0.82, "Assumed L1 rate: 100 kHz");
  SupportLabel(0.24, 0.76, "LHC fill 6360");
  canvas->SaveAs("avg_bitstr_occ_errors_vs_mu.eps");

  // =======================================================

  canvas->Clear();
  legend = new TLegend(0.24, 0.42, 0.34, 0.72);
  legend->SetTextFont(42);
  legend->SetTextSize(0.05);

  stack.reset(new HistStack{file, path, hist_titles});
  stack->setXAxisTitle("Lumi block");
  stack->setYAxisTitle("Average bandwidth occupancy");
  stack->setXAxisTicks(508);  // Don't cram the ticks on the x axis
  stack->createLegend(legend);
  stack->draw(canvas);
  legend->Draw("SAME");
  ATLASLabel(0.24, 0.88, "Pixel Internal");
  SupportLabel(0.24, 0.82, "Assumed L1 rate: 100 kHz");
  SupportLabel(0.24, 0.76, "LHC fill 6360");
  canvas->SaveAs("avg_bitstr_occ_errors_vs_lumi.eps");

  // =======================================================
  // Total bit-stream occupancy vs. pile-up

  // Reverting back to standard ATLAS style.
  canvas->SetLeftMargin(0.16);
  gStyle->SetTitleYOffset(1.4);

  hist_titles.clear();
  hist_titles.push_back("Bitstr_Occ_Tot_LB_B0");
  hist_titles.push_back("Bitstr_Occ_Tot_LB_B1");
  hist_titles.push_back("Bitstr_Occ_Tot_LB_B2");
  hist_titles.push_back("Bitstr_Occ_Tot_LB_ECA");
  hist_titles.push_back("Bitstr_Occ_Tot_LB_ECC");

  versus_pileup.clear();
  for (const auto& title : hist_titles) {
    versus_pileup.push_back(getVersusPileup(static_cast<TH1D*>(openCleanProfile(file, "run_339957/Pixel/Hits/Interactions_vs_lumi")), static_cast<TH1D*>(openCleanProfile(file, path + title))));
  }

  versus_pileup_projection.clear();
  for (auto& hist : versus_pileup) {
    hist->Rebin(2);
    versus_pileup_projection.push_back(hist->ProjectionX());
    versus_pileup_projection.back()->SetName(hist->GetName());
  }

  legend = new TLegend(0.2, 0.42, 0.3, 0.72);
  legend->SetTextFont(42);
  legend->SetTextSize(0.05);
  stack.reset(new HistStack{versus_pileup_projection});
  stack->setXAxisTitle("Average #mu per lumi block");
  stack->setYAxisTitle("Average bandwidth occupancy");
  stack->createLegend(legend);
  stack->draw(canvas);
  legend->Draw("SAME");
  ATLASLabel(0.2, 0.88, "Pixel Internal");
  SupportLabel(0.2, 0.82, "Assumed L1 rate: 100 kHz");
  SupportLabel(0.2, 0.76, "LHC fill 6360");
  canvas->SaveAs("avg_bitstr_occ_vs_mu.eps");

  // =======================================================
  // Total bit-stream occupancy vs. LB

  legend = new TLegend(0.8, 0.60, 0.9, 0.90);
  legend->SetTextFont(42);
  legend->SetTextSize(0.05);
  stack.reset(new HistStack{file, path, hist_titles});
  stack->setXAxisTitle("Lumi block");
  stack->setYAxisTitle("Average bandwidth occupancy");
  stack->setXAxisTicks(508);  // Don't cram the ticks on the x axis
  stack->createLegend(legend);
  stack->draw(canvas);
  legend->Draw("SAME");
  ATLASLabel(0.2, 0.88, "Pixel Internal");
  SupportLabel(0.2, 0.82, "Assumed L1 rate: 100 kHz");
  SupportLabel(0.2, 0.76, "LHC fill 6360");
  canvas->SaveAs("avg_bitstr_occ_vs_lumi.eps");

  // =======================================================
  // Histograms for error words

  auto hist = static_cast<TProfile*>(file->Get((path + "femcc_errorwords_merged_stat_B0").c_str()))->ProjectionX();
  hist->SetBarWidth(0.8);
  hist->SetBarOffset(0.1);
  hist->SetFillColor(kBlue+2);
  hist->GetXaxis()->SetTitle("# Error Words");
  hist->GetYaxis()->SetTitle("Frequency");
  hist->GetXaxis()->SetRangeUser(0, 6);
  hist->GetYaxis()->SetRangeUser(0, 0.6);

  // Draw a histogram in the background to point out the merging
  auto hist_bg = static_cast<TProfile*>(hist->Clone());
  hist_bg->SetBinContent(2, 1 - hist->GetBinContent(1));
  hist_bg->SetFillColor(kAzure-3);
  hist_bg->Draw("HIST BAR1");

  hist->Draw("HIST BAR1 SAME");
  canvas->SaveAs("errorwords_stat.eps");

  // Design adjustments for the 2D module maps
  canvas->SetRightMargin(0.10);
  gStyle->SetLabelSize(0.04, "xy");

  hist = static_cast<TH1D*>(file->Get((path + "femcc_errorwords_merged_B0").c_str()));
  hist->GetZaxis()->SetRangeUser(0, 5);
  hist->Draw();
  canvas->SaveAs("errorwords_merged.eps");

  hist = static_cast<TH1D*>(file->Get((path + "femcc_errorwords_B0").c_str()));
  hist->GetZaxis()->SetRangeUser(0, 5);
  hist->Draw();
  canvas->SaveAs("errorwords.eps");

  return 0;
}

void SupportLabel(double xpos, double ypos, const std::string& text) {
  TLatex p;
  p.SetNDC();
  p.SetTextFont(42);
  p.DrawLatex(xpos, ypos, text.c_str());
}

std::unique_ptr<TProfile> getVersusPileup(TH1D* pileup_hist, TH1D* hist) {
  if (!pileup_hist) return nullptr;
  if (!hist) return nullptr;
  auto versus_pileup = std::unique_ptr<TProfile>{
    new TProfile{(std::string("neu_") + hist->GetName()).c_str(),
                 (hist->GetTitle() + std::string(";pile-up;bitstream occ./module")).c_str(),
                 70, -0.5, 69.5}};

  for (unsigned int i = 1; i < pileup_hist->GetNbinsX() + 1; ++i) {
    auto pileup = pileup_hist->GetBinContent(i);
    auto occ = hist->GetBinContent(i);
    versus_pileup->Fill(pileup, occ);;
  }
  return versus_pileup;
}
