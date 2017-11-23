#ifndef HISTCLEANER_H_
#define HISTCLEANER_H_

class TFile;
class TProfile;

TProfile* openCleanProfile(TFile* file, const std::string& name);

#endif  // HISTCLEANER_H_
