//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQ_CONFIGREADER_H__
#define __TQ_CONFIGREADER_H__

#include <string>
#include <vector>
#include <set>

#include <TString.h>

#include "QFramework/TQTaggable.h"

class TEnv;
class TDirectory;

/**
 *
 * Provide access to configuration information
 * for a given package called 'name'.
 *
 * To use it, just instantiate an object of
 * this type. All lookups will be prepended with
 * the 'name' given.
 *
 * TQConfigReader config("MyTest");
 * TString value = config.get("MyValue","MyDefault");
 *
 * will look up 'Mytest.MyValue: ...' in the configuration file.
 *
 * The configuration file is searched in the following places:
 *
 * - ./.config.cfg if it exists
 */
class TQConfigReader {
protected:
  TString replace(TEnv *env, const TString& input);
  TEnv *_env;
  TString _name;
  TString _filename;
  TString _includePaths;
  TEnv* getEnv();

public:

  /// A list of all accessed entries.
  std::set<TString> s_accessed;

  const TString& getFilename();
  const TString& getName();

  explicit TQConfigReader(const TString& name, const TString& filename = ".config.cfg");
  virtual ~TQConfigReader();
 
  TString get(const TString& key, const TString& def);
  TString get(const TString& key, const char *def);
  int get(const TString& key, int def);
  double get(const TString& key, double def);
  bool get(const TString& key, bool def);
  
  void overrideInclude(TString& overridePath);

  std::vector<TString> getVString(const TString& key, const TString& delim = " ");
  std::vector<float> getVFloat(const TString& key, const TString& delim = " ");
  std::vector<double> getVDouble(const TString& key, const TString& delim = " ");
  std::vector<int> getVInt(const TString& key, const TString& delim = " ");

  void set(const TString& key, const TString& value);

  void dumpConfiguration();
  void dumpConfiguration(std::ostream& stream);
  void dumpConfiguration(TDirectory* const dir, TString const name = "configDB");
 
  std::vector<TString> checkConfiguration();
  std::vector<TString> variables(const TString& name) ;
 
  void resolve();

  TQTaggable* exportAsTags();
  bool exportAsTags(TQTaggable* tags);
  bool isValid();

};


#endif // __TQ_CONFIGREADER_H__
