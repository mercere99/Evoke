//  This file is part of Evoke, https://github.com/mercere99/Evoke/
//  Copyright (C) Michigan State University, 2016.
//  Released under the MIT Software license; see doc/LICENSE
//
//
//  This file contains definitions that will be useful throughout the Evoke project
//

#ifndef EVOKE_DEFS_H
#define EVOKE_DEFS_H

// If we haven't specified a config file in out compiler options, set the default here.
#ifndef EMP_CONFIG_FILE
#define EMP_CONFIG_FILE "source/evoke_config.h"
#endif

#include "emp/config/config.hpp"
#include "emp/hardware/AvidaGP.hpp"
#include "emp/in_progress/Trait.hpp"

namespace evoke {

  // Pre-declarations needed for types below.
  struct World;
  template <typename HARDWARE> class OrgControl;

  // Simple typedefs...
  using dControl = OrgControl<emp::AvidaGP>;
  
  // Construction of the specific Phenotype types.
  typedef emp::TraitManager<int, double, bool, std::string,
                            std::vector<int>, std::vector<double>,
                            std::vector<bool>, std::vector<std::string> >  PhenotypeManager;
  
  typedef emp::TraitSet<int, double, bool, std::string,
                      std::vector<int>, std::vector<double>,
                      std::vector<bool>, std::vector<std::string> >  Phenotype;

}

#endif
