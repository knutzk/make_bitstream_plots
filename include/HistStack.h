#ifndef HISTSTACK_H_
#define HISTSTACK_H_

#include <memory>
#include <string>
#include <vector>

class TCanvas;
class TH1D;
class TLegend;

class HistStack {
 public:
  /**
   * Initialise the container, given a vector of unique pointers
   * to TH1D objects. This effectively releases the histograms in
   * that vector and transfers ownership to this object. This
   * constructor also takes care of basic settings, such as
   * colors and markers. An additional maximal value for the
   * X-axis can be given to limit the plotting range.
   * @param histos Vector with TH1D unique pointers
   * @param x_max Optional maximal x value for plotting
   */
  HistStack(std::vector<std::unique_ptr<TH1D> >& histos, double x_max = 0);

  /// Create the legend based on the containing histograms.
  void createLegend(TLegend* legend);

  /// Draw all histograms on the given canvas. If no custom
  /// maximal value is set via setComfortableMax(), this function
  /// also determines the optimal plotting range.
  void draw(TCanvas* canvas);

  /// Get the maximal entry in all histograms.
  double getMax() const;

  /// Print a table of all histogram bin contents.
  std::string printTable() const;

  /// Set a comfortable maximum for all histograms. This adds a
  /// certain overhead to the given value 'max' and then rounds
  /// this overhead to a nice value.
  void setComfortableMax(double max);

  /// Set the X-axis title for all histograms simultaneously.
  void setXAxisTitle(const std::string& title);

  /// Set the X-axis ticks for all histograms. This internally
  /// calls the TH1D::SetNdivisions() function.
  void setXAxisTicks(unsigned int ticks);

  /// Set the Y-axis title for all histograms simultaneously.
  void setYAxisTitle(const std::string& title);

  /// Shift histograms in this container by a given fraction
  /// (negative: to the left, positive: to the right) to avoid
  /// overlap of data points. The size of the given vector must
  /// be identical to the number of histograms. Values are in
  /// fractions of the width of the first bin (e.g. '0.1' for bin
  /// width 5 shifts by a total of 0.5).
  void shift(const std::vector<float>& shift_values);

 private:
  /// The histogram titles, to be used in the legend.
  std::vector<std::string> titles_;

  /// Vector of all histograms this container holds.
  std::vector<std::unique_ptr<TH1D>> histograms_;

  /// Boolean whether a custom maximal value was set.
  bool has_custom_max_{false};
};

#endif  // HISTSTACK_H_
