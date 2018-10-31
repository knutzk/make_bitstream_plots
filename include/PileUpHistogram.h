#ifndef PILE_UP_HISTOGRAM_H_
#define PILE_UP_HISTOGRAM_H_

#include <memory>
#include <set>
#include <string>

class TFile;
class TH1D;

class PileUpHistogram {
public:
  /// Create a pile-up histogram from one file. The input values
  /// are taken from the given histogram in the given path
  PileUpHistogram(TFile* file, const std::string& path, const std::string& histo_name);

  /// Perform the actual fill method for the histogram. This maps
  /// luminosity blocks to pile-up values.
  void fillHisto();

  /// Veto a set of luminosity blocks that don't contain "good"
  /// values, e.g. when the stream is not yet active.
  void vetoLumiBlocks(const std::set<int> lbs);

  /// Set the range of the pile-up axis.
  void setPileUpRange(float min, float max);

  /// Get a non-const pointer to the underlying histogram.
  TH1D* getHisto() { return m_histo.get(); }

  /// Get the unique pointer to the underlying histogram.
  std::unique_ptr<TH1D> getHistoUnique() { return std::move(m_histo); }

private:
  std::string m_path{""};
  std::string m_histo_name{""};
  std::set<int> m_vetoed_lbs{};
  double m_pile_up_min{0.};
  double m_pile_up_max{20.};
  int m_pile_up_bins{10};
  TFile* m_file{nullptr};
  std::unique_ptr<TH1D> m_histo{nullptr};
};

#endif  // PILE_UP_HISTOGRAM_H_
