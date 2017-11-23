#ifndef HISTSTACK_H_
#define HISTSTACK_H_

#include <string>
#include <vector>

class TCanvas;
class TFile;
class TH1D;
class TLegend;

class HistStack {
 public:
  HistStack(TFile* file, const std::string& path, const std::vector<std::string>& titles);

  HistStack(std::vector<TH1D*> hists);

  void createLegend(TLegend* legend);

  void draw(TCanvas* canvas);

  double getMax();

  void init();

  void setComfortableMax(double max);

  void setXAxisTitle(const std::string& title);

  void setXAxisTicks(unsigned int ticks);

  void setYAxisTitle(const std::string& title);

  std::vector<std::string> titles_;
  std::vector<TH1D*> histograms_;
};

#endif  // HISTSTACK_H_
