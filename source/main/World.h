#ifndef EVOKE_WORLD_H
#define EVOKE_WORLD_H

////////////////////////////////////////////////////////////////////////////////////////////
//
//  The World struct maintains all needed information to describe a single world instance.
//
//  The main goal of this struct is to have only a single entity to pass around when 
//  world information is needed.  Most runs will probably only have a single world, but
//  multiple worlds should be possible.
//


#include "config/config.h"
#include "tools/Random.h"

namespace evoke {
  
  struct World {
    const double width = 512.0;       // Size of the world.
    const double height = 512.0;
    const double repro_prob = 0.003;  // Chance of legal org to replicate each update.
    const double drift = 0.15;        // Amount of Brownian motion.
    const double org_radius = 3.0;
    
    emp::Config config;
    emp::Physics2D<evoke::dBody, evoke::dControl> physics;
    emp::Random random;    

    World() : physics(width, height) { ; }

    void Init() {
      // Initialize an organism in the middle of the world.
      const emp::Point<double>  mid_point( width / 2.0, height / 2.0 );
      auto org = new evoke::dBody(evoke::dCircle(mid_point, org_radius), NULL);
      physics.AddBody(org);
    }

    void Update() {
      // Take a single timestep on the world physics.
      physics.Update();

      // Test which organisms should replicate.
      auto & body_set = physics.GetBodySet();

      // Loop through all bodies to see which ones should replicate.
      for (auto * body : body_set) {
        // Add a small amount of Brownian motion...
        body->IncSpeed( emp::Angle(random.GetDouble() * (2.0 * emp::PI)).GetPoint(drift) );
      
        // Bodies cannot repriduce IF:
        // * They are already reproducing
        // * They are under too much pressure
        // * They are attached to too many bodies.
        if (body->IsReproducing()
            || body->GetPressure() > 1.0
            || body->GetLinkCount() >= 3) continue;

        if (random.P(repro_prob) || body_set.size() == 1) {
          emp::Angle repro_angle(random.GetDouble(2.0 * emp::PI));
          auto * new_body = body->BuildOffspring( repro_angle.GetPoint(0.1) );

          // For the moment, assume 95% chance of faithful copy.
          if (random.P(0.95)) new_body->SetColorID(body->GetColorID());
          else new_body->SetColorID( random.GetInt(360) );
          
          physics.AddBody( new_body );
        }
      }

    }
  };

};

#endif
