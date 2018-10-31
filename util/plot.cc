#include "HistStack.h"
#include "PileUpHistogram.h"
#include "DirectoryParser.h"
#include "AtlasStyle.h"
#include "AtlasLabels.h"

#include "TLatex.h"
#include "TProfile.h"
#include "TH1D.h"
#include "TFile.h"
#include "TLegend.h"
#include "TCanvas.h"

#include <fstream>
#include <iostream>
#include <memory>

namespace {
void SupportLabel(double xpos, double ypos, const std::string& text) {
  TLatex p;
  p.SetNDC();
  p.SetTextFont(42);
  p.DrawLatex(xpos, ypos, text.c_str());
}
}  // namespace



int main(int argc, char** argv) {
  SetAtlasStyle();

  // Sanitize the user input.
  // ---------------------------------------------------------
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

  // Extract infos like run number and stream.
  // ---------------------------------------------------------
  std::string fill_number = "???";
  if (std::string(argv[1]).find("339849") != std::string::npos) fill_number = "6358";
  if (std::string(argv[1]).find("356124") != std::string::npos) fill_number = "6953";

  std::string stream = "???";
  if (std::string(argv[1]).find("express") != std::string::npos) stream = "express_express";
  if (std::string(argv[1]).find("zerobias") != std::string::npos) stream = "physics_ZeroBias";
  if (std::string(argv[1]).find("enhanced") != std::string::npos) stream = "physics_EnhancedBias";

  // Set up the canvases and legends
  // ---------------------------------------------------------
  TCanvas canvas{"canvas", "canvas", 600, 600};
  TLegend legend{0.78, 0.6, 0.93, 0.9};
  legend.SetTextFont(42);
  legend.SetTextSize(0.05);
  TLegend left_legend{0.24, 0.42, 0.39, 0.72};
  left_legend.SetTextFont(42);
  left_legend.SetTextSize(0.05);

  std::set<int> vetoed_lbs;
  for (int i = 0; i < 246; ++i) {
    vetoed_lbs.insert(i);
  }

  // Here the actual setup of the plots is done.
  // -------------------------------------------------------
  const double pile_up_min = 22.5;
  const double pile_up_max = 62.5;
  const int n_bins_from_zero = std::floor((pile_up_max + 2.5)/5);

  std::vector<std::unique_ptr<TH1D> > reduced_hists;

  // What we do here is the following: we look for all
  // module-granularity histograms that match the given wildcard.
  // For these histograms, we map from luminosity vs. bandwidth
  // usage to pile-up vs. bandwidth usage by using the
  // luminosity/pile-up relation stored under Pixel/Hits/. After
  // getting histograms for all modules individually, we reduce
  // this infromation to _one_ histogram per given wildcard (e.g.
  // for layers).
  auto add_to_reduced_hists = [&] (const std::string& wildcard, const std::string& title) {
    DirectoryParser parser{file, path, wildcard};
    TProfile prof{(title).c_str(), ("prof_" + title).c_str(), n_bins_from_zero, -2.5, pile_up_max, "s"};
    for (const auto& module : parser.modules) {
      PileUpHistogram hist{file, path, module};
      hist.vetoLumiBlocks(vetoed_lbs);
      hist.setPileUpRange(pile_up_min, pile_up_max);
      hist.fillHisto();
      for (int i = 1; i <= hist.getHisto()->GetNbinsX(); ++i) {
        if (hist.getHisto()->GetBinContent(i) == 0) continue;
        prof.Fill(hist.getHisto()->GetBinCenter(i), hist.getHisto()->GetBinContent(i));
      }
    }
    reduced_hists.emplace_back(std::unique_ptr<TH1D>(prof.ProjectionX()));
    reduced_hists.back()->SetName(prof.GetName());
    reduced_hists.back()->GetXaxis()->SetRangeUser(12.5, pile_up_max);
  };

  add_to_reduced_hists("^L0", "L0");
  add_to_reduced_hists("^L1", "L1");
  add_to_reduced_hists("^L2", "L2");
  add_to_reduced_hists("^ECA", "ECA");
  add_to_reduced_hists("^ECC", "ECC");
  add_to_reduced_hists("^LI.*_[AC][^(7|8)]_", "IBL2D");
  add_to_reduced_hists("^LI.*_[AC][78]_", "IBL3D");

  // Plots for: total bit-stream usage
  // -------------------------------------------------------
  HistStack stack{reduced_hists};
  stack.setXAxisTitle("Average #mu per lumi block");
  stack.setYAxisTitle("Average bandwidth usage");
  stack.setComfortableMax(0.7);
  stack.setXAxisTicks(210);
  stack.createLegend(&left_legend);
  stack.draw(&canvas);
  left_legend.Draw("SAME");
  ATLASLabel(0.2, 0.88, "Pixel Internal");
  SupportLabel(0.2, 0.82, "Assumed L1 rate: 100 kHz");
  SupportLabel(0.2, 0.76, "Fill " + fill_number + ", " + stream);
  canvas.SaveAs("output/avg_bitstr_occ_vs_mu.eps");
  canvas.SaveAs("output/avg_bitstr_occ_vs_mu.pdf");
  canvas.SaveAs("output/avg_bitstr_occ_vs_mu.png");
  left_legend.Clear();

  std::cout << stack.printTable() << std::endl;

  return 0;
}
