#include "DirectoryParser.h"
#include "TFile.h"
#include "TKey.h"
#include "TClass.h"
#include "TProfile.h"

#include <iostream>
#include <regex>

DirectoryParser::DirectoryParser(TFile* file, const std::string& path, const std::string& wildcard) {
  auto dir = file->GetDirectory(std::string(path + "Errors/Modules_BitStr_Occ_Tot/").c_str());

  TIter nextkey(dir->GetListOfKeys());
  TKey *key = nullptr;

  // Loop through the pixel components (IBL, L0, etc).
  while ((key = static_cast<TKey*>(nextkey()))) {
    TObject *obj = key->ReadObj();
    if (!obj->IsA()->InheritsFrom(TDirectory::Class())) continue;

    TIter nextkey2(dir->GetDirectory(obj->GetName())->GetListOfKeys());
    TKey *key2 = nullptr;

    // Loop through the staves/structures.
    while ((key2 = static_cast<TKey*>(nextkey2()))) {
      TObject *obj2 = key2->ReadObj();
      if (!obj2->IsA()->InheritsFrom(TDirectory::Class())) continue;

      TIter nextkey3(dir->GetDirectory(std::string(std::string(obj->GetName()) + "/" + obj2->GetName()).c_str())->GetListOfKeys());
      TKey *key3 = nullptr;

      // Loop through the actual modules.
      while ((key3 = static_cast<TKey*>(nextkey3()))) {
        TObject *obj3 = key3->ReadObj();
        if (!obj3->IsA()->InheritsFrom(TProfile::Class())) continue;

        auto full_name = std::string(obj->GetName()) + "/" + obj2->GetName() + "/" + obj3->GetName();

        // Make sure that the full_name matches the wildcard pattern.
        std::smatch match;
        std::regex_search(full_name, match, std::regex(wildcard));
        if (match.empty()) continue;
        // std::cout << "Matched pattern: " << full_name << std::endl;
        modules.insert(full_name);
      }
    }
  }

  std::cout << "Found " << modules.size() << " modules matching pattern: " << wildcard << std::endl;
}
