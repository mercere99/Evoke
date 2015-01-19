#ifndef EVOKE_DEFS_H
#define EVOKE_DEFS_H

////////////////////////////////////////////////////////////////////////////////////////
//
//  This file contains definitions that will be useful throughout the Evoke project
//

#include "tools/config.h"
#include "tools/Traits.h"

#define EMP_CONFIG_FILE "evoke_config.h"

namespace emp {

  // Pre-declarations needed for types below.
  class World;

  // Construction of the specific Phenotype types.
  typedef TraitManager<int, double, bool, std::string,
                       std::vector<int>, std::vector<double>,
                       std::vector<bool>, std::vector<std::string> >  PhenotypeManager;

  typedef TraitSet<int, double, bool, std::string,
                   std::vector<int>, std::vector<double>,
                   std::vector<bool>, std::vector<std::string> >  Phenotype;

};

#endif
