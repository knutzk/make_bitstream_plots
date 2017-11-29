#include "HistStack.h"
#include "PileupHistStack.h"
#include "HistCleaner.h"
#include "AtlasStyle.h"
#include "AtlasLabels.h"

#include "TLatex.h"
#include "TProfile.h"
#include "TH1D.h"
#include "TFile.h"
#include "TLegend.h"
#include "TCanvas.h"

#include <iostream>
#include <memory>

void SupportLabel(double xpos, double ypos, const std::string& text);

int main(int argc, char** argv) {
  SetAtlasStyle();

  if (argc != 3) {
    std::cerr << "Wrong number of positional arguments" << std::endl;
    std::cerr << "Usage: ./plot [input file] [run number]" << std::endl;
    return -1;
  }

  auto file = TFile::Open(argv[1], "READ");
  if (!file) return -1;
  std::string path = "run_" + std::string(argv[2]) + "/Pixel/";
  if (!file->GetDirectory(path.c_str())) {
    std::cerr << "Directory " + path + " does not exist. Check run number" << std::endl;
    return -1;
  }

  std::string fill_number = "???";
  if (std::string(argv[2]).find("339849") != std::string::npos) fill_number = "6358";

  // Canvases and legends
  TCanvas canvas{"canvas", "canvas", 600, 600};
  TLegend legend{0.8, 0.6, 0.9, 0.9};
  legend.SetTextFont(42);
  legend.SetTextSize(0.05);
  TLegend left_legend{0.24, 0.42, 0.34, 0.72};
  left_legend.SetTextFont(42);
  left_legend.SetTextSize(0.05);

  // =======================================================
  // =======================================================
  // Plots for: bit-stream usage by FE/MCC Errors
  // =======================================================

  // We need more space on the left for these plots.
  canvas.SetLeftMargin(0.2);
  gStyle->SetTitleYOffset(1.8);

  std::vector<std::string> hist_titles;
  hist_titles.push_back("Errors/Bitstr_Occ_Errors_LB_B0");
  hist_titles.push_back("Errors/Bitstr_Occ_Errors_LB_B1");
  hist_titles.push_back("Errors/Bitstr_Occ_Errors_LB_B2");
  hist_titles.push_back("Errors/Bitstr_Occ_Errors_LB_ECA");
  hist_titles.push_back("Errors/Bitstr_Occ_Errors_LB_ECC");

  auto pileup_stack = std::make_unique<PileupHistStack>(PileupHistStack{file, path, hist_titles});
  pileup_stack->setXAxisTitle("Average #mu per lumi block");
  pileup_stack->setYAxisTitle("Average error bandwidth usage");
  pileup_stack->setXAxisTicks(210);
  pileup_stack->createLegend(&legend);
  pileup_stack->draw(&canvas);
  legend.Draw("SAME");
  ATLASLabel(0.24, 0.88, "Pixel Internal");
  SupportLabel(0.24, 0.82, "Assumed L1 rate: 100 kHz");
  SupportLabel(0.24, 0.76, "Fill " + fill_number);
  canvas.SaveAs("output/avg_bitstr_occ_errors_vs_mu.eps");
  canvas.SaveAs("output/avg_bitstr_occ_errors_vs_mu.pdf");
  canvas.SaveAs("output/avg_bitstr_occ_errors_vs_mu.png");
  legend.Clear();

  auto stack = std::make_unique<HistStack>(HistStack{file, path, hist_titles, 1200});
  stack->setXAxisTitle("Lumi block");
  stack->setYAxisTitle("Average error bandwidth usage");
  stack->setXAxisTicks(508);  // Don't cram the ticks on the x axis
  stack->createLegend(&left_legend);
  stack->draw(&canvas);
  left_legend.Draw("SAME");
  ATLASLabel(0.24, 0.88, "Pixel Internal");
  SupportLabel(0.24, 0.82, "Assumed L1 rate: 100 kHz");
  SupportLabel(0.24, 0.76, "Fill " + fill_number);
  canvas.SaveAs("output/avg_bitstr_occ_errors_vs_lumi.eps");
  canvas.SaveAs("output/avg_bitstr_occ_errors_vs_lumi.pdf");
  canvas.SaveAs("output/avg_bitstr_occ_errors_vs_lumi.png");
  left_legend.Clear();

  // =======================================================
  // =======================================================
  // Plots for: total bit-stream usage
  // =======================================================

  // Reverting back to standard ATLAS style.
  left_legend.SetX1NDC(0.20);
  left_legend.SetX2NDC(0.30);
  canvas.SetLeftMargin(0.16);
  gStyle->SetTitleYOffset(1.4);

  hist_titles.clear();
  hist_titles.push_back("Errors/Bitstr_Occ_Tot_LB_B0");
  hist_titles.push_back("Errors/Bitstr_Occ_Tot_LB_B1");
  hist_titles.push_back("Errors/Bitstr_Occ_Tot_LB_B2");
  hist_titles.push_back("Errors/Bitstr_Occ_Tot_LB_ECA");
  hist_titles.push_back("Errors/Bitstr_Occ_Tot_LB_ECC");

  pileup_stack.reset(new PileupHistStack{file, path, hist_titles});
  pileup_stack->setXAxisTitle("Average #mu per lumi block");
  pileup_stack->setYAxisTitle("Average bandwidth usage");
  pileup_stack->setComfortableMax(0.7);
  pileup_stack->setXAxisTicks(210);
  pileup_stack->createLegend(&left_legend);
  pileup_stack->draw(&canvas);
  left_legend.Draw("SAME");
  ATLASLabel(0.2, 0.88, "Pixel Internal");
  SupportLabel(0.2, 0.82, "Assumed L1 rate: 100 kHz");
  SupportLabel(0.2, 0.76, "Fill " + fill_number);
  canvas.SaveAs("output/avg_bitstr_occ_vs_mu.eps");
  canvas.SaveAs("output/avg_bitstr_occ_vs_mu.pdf");
  canvas.SaveAs("output/avg_bitstr_occ_vs_mu.png");
  left_legend.Clear();
  pileup_stack->printTable();

  stack.reset(new HistStack{file, path, hist_titles, 1200});
  stack->setXAxisTitle("Lumi block");
  stack->setYAxisTitle("Average bandwidth usage");
  stack->setXAxisTicks(508);  // Don't cram the ticks on the x axis
  stack->setComfortableMax(0.7);
  stack->createLegend(&legend);
  stack->draw(&canvas);
  legend.Draw("SAME");
  ATLASLabel(0.2, 0.88, "Pixel Internal");
  SupportLabel(0.2, 0.82, "Assumed L1 rate: 100 kHz");
  SupportLabel(0.2, 0.76, "Fill " + fill_number);
  canvas.SaveAs("output/avg_bitstr_occ_vs_lumi.eps");
  canvas.SaveAs("output/avg_bitstr_occ_vs_lumi.pdf");
  canvas.SaveAs("output/avg_bitstr_occ_vs_lumi.png");
  legend.Clear();

  // =======================================================
  // =======================================================
  // Plots for: error words (on L0)
  // =======================================================

  auto hist = static_cast<TProfile*>(file->Get((path + "Errors/femcc_errorwords_merged_stat_B0").c_str()))->ProjectionX();
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
  canvas.SaveAs("output/errorwords_stat.eps");
  canvas.SaveAs("output/errorwords_stat.pdf");
  canvas.SaveAs("output/errorwords_stat.png");

  // Design adjustments for the 2D module maps
  canvas.SetRightMargin(0.10);
  gStyle->SetLabelSize(0.04, "xy");

  hist = static_cast<TH1D*>(file->Get((path + "Errors/femcc_errorwords_merged_B0").c_str()));
  hist->GetZaxis()->SetRangeUser(0, 5);
  hist->Draw();
  canvas.SaveAs("output/errorwords_merged.eps");
  canvas.SaveAs("output/errorwords_merged.pdf");
  canvas.SaveAs("output/errorwords_merged.png");

  hist = static_cast<TH1D*>(file->Get((path + "Errors/femcc_errorwords_B0").c_str()));
  hist->GetZaxis()->SetRangeUser(0, 5);
  hist->Draw();
  canvas.SaveAs("output/errorwords.eps");
  canvas.SaveAs("output/errorwords.pdf");
  canvas.SaveAs("output/errorwords.png");

  return 0;
}

void SupportLabel(double xpos, double ypos, const std::string& text) {
  TLatex p;
  p.SetNDC();
  p.SetTextFont(42);
  p.DrawLatex(xpos, ypos, text.c_str());
}
