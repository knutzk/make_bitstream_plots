HistStack::HistStack(std::vector<TH1D*> hists)
  : histograms_(hists) {
  for (const auto& hist : hists) {
    titles_.push_back(hist->GetName());
  }

  this->init();
}

HistStack::HistStack(TFile* file, const std::string& path, const std::vector<std::string>& titles)
  : titles_(titles) {
  for (const auto& title : titles_) {
    auto hist = static_cast<TH1D*>(file->Get((path + title).c_str()));
    if (!hist) {
      std::cerr << "Histogram " << title << "not found. Skipping" << std::endl;
      continue;
    }
    histograms_.emplace_back(hist);
  }

  this->init();
}

void HistStack::createLegend(TLegend* legend) {
  auto get_suffix = [](const std::string& s) {
    auto pos = s.rfind("_");
    return s.substr(pos + 1, s.length() - pos);
  };

  for (const auto& hist : histograms_) {
    legend->AddEntry(hist, get_suffix(std::string(hist->GetName())).c_str());
  }
}

void HistStack::draw(TCanvas* canvas) {
  this->setComfortableMax(1.4*this->getMax());
  for (const auto& hist : histograms_) {
    if (hist == histograms_.front()) {
      hist->Draw("E");
    } else {
      hist->Draw("PE SAME");
    }
  }
}

double HistStack::getMax() {
  double max{0.};
  for (const auto& hist : histograms_) {
    for (unsigned int i = 1; i < hist->GetNbinsX() + 1; ++i) {
      if (hist->GetBinContent(i) > max) {
        max = hist->GetBinContent(i);
      }
    }
  }
  return max;
}

void HistStack::init() {
  int marker_index = 0;
  int color_index = 0;

  for (auto& hist : histograms_) {
    color_index++;
    marker_index++;
    if (color_index == 5) color_index++;
    if (marker_index == 3) marker_index++;
    if (hist->GetNbinsX() > 1000) hist->Rebin(100);
    hist->GetXaxis()->SetRangeUser(0, 1800);
    hist->SetMarkerColor(color_index);
    hist->SetLineColor(color_index);
    hist->SetMarkerStyle(19 + marker_index);
  }
}

void HistStack::setComfortableMax(double max) {
  for (auto& hist : histograms_) {
    hist->GetYaxis()->SetRangeUser(0, max);
  }
}
