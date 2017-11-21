#include "HistStack.h"
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

  // =======================================================

  auto canvas = new TCanvas;
  auto legend = new TLegend(0.2, 0.45, 0.3, 0.75);
  legend->SetTextFont(42);
  legend->SetTextSize(0.05);

  std::vector<std::string> hist_titles;
  hist_titles.push_back("Bitstr_Occ_Errors_LB_B0");
  hist_titles.push_back("Bitstr_Occ_Errors_LB_B1");
  hist_titles.push_back("Bitstr_Occ_Errors_LB_B2");
  hist_titles.push_back("Bitstr_Occ_Errors_LB_ECA");
  hist_titles.push_back("Bitstr_Occ_Errors_LB_ECC");
  HistStack stack{file, path, hist_titles};
  stack.createLegend(legend);
  stack.draw(canvas);
  legend->Draw("SAME");
  ATLASLabel(0.2, 0.88, "Pixel Internal");
  SupportLabel(0.2, 0.82, "run 339,957");
  canvas->SaveAs("avg_bitstr_occ_errors.eps");

  // =======================================================

  hist_titles.clear();
  hist_titles.push_back("Bitstr_Occ_Tot_LB_B0");
  hist_titles.push_back("Bitstr_Occ_Tot_LB_B1");
  hist_titles.push_back("Bitstr_Occ_Tot_LB_B2");
  hist_titles.push_back("Bitstr_Occ_Tot_LB_ECA");
  hist_titles.push_back("Bitstr_Occ_Tot_LB_ECC");

  std::vector<std::unique_ptr<TProfile>> versus_pileup;
  for (const auto& title : hist_titles) {
    versus_pileup.push_back(getVersusPileup(static_cast<TH1D*>(file->Get("run_339957/Pixel/Hits/Interactions_vs_lumi")), static_cast<TH1D*>(file->Get((path + title).c_str()))));
  }

  std::vector<TH1D*> versus_pileup_projection;
  for (auto& hist : versus_pileup) {
    hist->Rebin(2);
    versus_pileup_projection.push_back(hist->ProjectionX());
    versus_pileup_projection.back()->SetName(hist->GetName());
  }

  legend = new TLegend(0.2, 0.45, 0.3, 0.75);
  legend->SetTextFont(42);
  legend->SetTextSize(0.05);
  HistStack stack2{versus_pileup_projection};
  stack2.createLegend(legend);
  stack2.draw(canvas);
  legend->Draw("SAME");
  ATLASLabel(0.2, 0.88, "Pixel Internal");
  SupportLabel(0.2, 0.82, "run 339,957");
  canvas->SaveAs("avg_bitstr_occ_vs_mu.eps");

  // =======================================================

  legend = new TLegend(0.8, 0.60, 0.9, 0.90);
  legend->SetTextFont(42);
  legend->SetTextSize(0.05);
  HistStack stack3{file, path, hist_titles};
  stack3.createLegend(legend);
  stack3.draw(canvas);
  legend->Draw("SAME");
  ATLASLabel(0.2, 0.88, "Pixel Internal");
  SupportLabel(0.2, 0.82, "run 339,957");
  canvas->SaveAs("avg_bitstr_occ.eps");

  // // =======================================================

  // auto hist = static_cast<TProfile*>(file->Get((path + "femcc_errorwords_merged_stat_B0").c_str()))->ProjectionX();
  // hist->SetBarWidth(0.8);
  // hist->SetBarOffset(0.1);
  // hist->SetFillColor(kBlue);
  // hist->GetXaxis()->SetTitle("# Error Words");
  // hist->GetYaxis()->SetTitle("Frequency");
  // hist->GetXaxis()->SetRangeUser(0, 6);
  // hist->GetYaxis()->SetRangeUser(0, 0.6);
  // hist->SetBinContent(2, 1 - hist->GetBinContent(1));
  // hist->Draw("HIST BAR1");
  // canvas->SaveAs("errorwords_stat.eps");

  // canvas->SetRightMargin(0.10);
  // gStyle->SetLabelSize(12, "xyz");
  // hist = static_cast<TH1D*>(file->Get((path + "femcc_errorwords_merged_B0").c_str()));
  // hist->GetZaxis()->SetRangeUser(0, 5);
  // hist->Draw();
  // canvas->SaveAs("errorwords_merged.eps");

  // hist = static_cast<TH1D*>(file->Get((path + "femcc_errorwords_B0").c_str()));
  // hist->GetZaxis()->SetRangeUser(0, 5);
  // hist->Draw();
  // canvas->SaveAs("errorwords.eps");

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
