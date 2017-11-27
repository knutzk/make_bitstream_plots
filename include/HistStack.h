#ifndef HISTSTACK_H_
#define HISTSTACK_H_

#include <memory>
#include <string>
#include <vector>

class TCanvas;
class TFile;
class TH1D;
class TLegend;

class HistStack {
 public:
  inline HistStack(const std::vector<std::string>& titles) : titles_(titles) {};

  HistStack(TFile* file, const std::string& path, const std::vector<std::string>& titles, double x_max = 0);

  void createLegend(TLegend* legend);

  void draw(TCanvas* canvas);

  double getMax();

  void init(double x_max = 0);

  void setComfortableMax(double max);

  void setXAxisTitle(const std::string& title);

  void setXAxisTicks(unsigned int ticks);

  void setYAxisTitle(const std::string& title);

  std::vector<std::string> titles_;
  std::vector<std::string> titles_short_;
  std::vector<std::unique_ptr<TH1D>> histograms_;

private:
  bool has_custom_max_{false};
};

#endif  // HISTSTACK_H_
