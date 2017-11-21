#ifndef HISTSTACK_H_
#define HISTSTACK_H_

class HistStack {
 public:
  HistStack(TFile* file, const std::string& path, const std::vector<std::string>& titles);

  HistStack(std::vector<TH1D*> hists);

  void createLegend(TLegend* legend);

  void draw(TCanvas* canvas);

  double getMax();

  void init();

  void setComfortableMax(double max);

  std::vector<std::string> titles_;
  std::vector<TH1D*> histograms_;
};

#endif  // HISTSTACK_H_
