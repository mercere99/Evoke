#ifndef EVOKE_DEFS_H
#define EVOKE_DEFS_H

//////////////////////////////////////////////////////////////////////////////////////////
//
//  This file contains definitions that will be useful throughout the Evoke project
//


// If we haven't specified a config file in out compiler options, set the default here.
#ifndef EMP_CONFIG_FILE
#define EMP_CONFIG_FILE "source/evoke_config.h"
#endif

#include "geometry/Body2D.h"
#include "tools/config.h"
#include "tools/Trait.h"

namespace evoke {

  // Pre-declarations needed for types below.
  struct World;
  class OrgControl;

  typedef double dBase;
  typedef OrgControl dControl;
  typedef emp::CircleBody2D< dControl, dBase > dBody;
  
  // Construction of the specific Phenotype types.
  typedef emp::TraitManager<int, double, bool, std::string,
                            std::vector<int>, std::vector<double>,
                            std::vector<bool>, std::vector<std::string> >  PhenotypeManager;
  
  typedef emp::TraitSet<int, double, bool, std::string,
                      std::vector<int>, std::vector<double>,
                      std::vector<bool>, std::vector<std::string> >  Phenotype;

};

#endif
