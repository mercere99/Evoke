//  This file is part of Evoke, https://github.com/mercere99/Evoke/
//  Copyright (C) Michigan State University, 2016.
//  Released under the MIT Software license; see doc/LICENSE
//
//
//  The World struct maintains all needed information to describe a single world instance.
//
//  The main goal of this struct is to have only a single entity to pass around when 
//  world information is needed.  Most runs will probably only have a single world, but
//  multiple worlds should be possible.
//


#ifndef EVOKE_WORLD_H
#define EVOKE_WORLD_H


#include "config/config.h"
#include "tools/Random.h"

namespace evoke {
  
  struct World {
    const double width = 512.0;       // Size of the world.
    const double height = 512.0;
    const double drift = 0.15;        // Amount of Brownian motion.
    const double repro_prob = 0.003;  // Chance of legal org to replicate each update.
    const double pop_pressure = 1.0;  // How much pressure before an organism dies?
    double org_radius;                // How big should each cell get?
    int max_link_count;               // How many links can an org have?
    
    emp::Config config;
    emp::Physics2D<evoke::dBody> physics;
    emp::Random random;    

    World() : org_radius(4.0), max_link_count(3), physics(width, height) { ; }

    void Init() {
      // Initialize an organism in the middle of the world.
      const emp::Point<double>  mid_point( width / 2.0, height / 2.0 );
      auto org = new evoke::dBody(evoke::dCircle(mid_point, org_radius));
      physics.AddBody(org);
    }

    void Reset() {
      // Clear everything currently in the world.
      physics.Clear();

      // Re-initialize a new organism in the middle of the world.
      Init();
    }

    void Update() {
      // Take a single timestep on the world physics.
      physics.Update();

      // Test which organisms should replicate.
      auto & body_set = physics.GetBodySet();
      emp::vector<evoke::dBody *> new_bodies;
      
      // Loop through all bodies to see which ones should replicate.
      for (auto * body : body_set) {
        // Add a small amount of Brownian motion...
        body->IncSpeed( emp::Angle(random.GetDouble() * (2.0 * emp::PI)).GetPoint(drift) );
      
        // Bodies cannot repriduce IF:
        // * They are already reproducing
        // * They are under too much pressure
        // * They are attached to too many bodies.
        if (body->IsReproducing()
            || body->GetPressure() > pop_pressure
            || body->GetLinkCount() >= max_link_count) continue;

        if (random.P(repro_prob) || (body_set.size() < 4 && random.P(repro_prob*3.0))) {
          emp::Angle repro_angle(random.GetDouble(2.0 * emp::PI));
          auto * new_body = body->BuildOffspring( repro_angle.GetPoint(0.1) );

          // For the moment, assume 95% chance of faithful copy.
          if (random.P(0.95)) new_body->SetColorID(body->GetColorID());
          else new_body->SetColorID( random.GetInt(360) );
          
          new_bodies.push_back(new_body); // Mark this body to be added.
        }
      }

      // Now add all of the new bodies into the world.
      for (auto new_body : new_bodies) {
        physics.AddBody( new_body );
      }

    }
  };

};

#endif
