//  This file is part of Evoke, https://github.com/mercere99/Evoke/
//  Copyright (C) Michigan State University, 2016-2025.
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

#include "emp/base/Ptr.hpp"
#include "emp/config/config.hpp"
#include "emp/geometry/Physics2D.hpp"
#include "emp/geometry/Surface.hpp"
#include "emp/math/Random.hpp"
#include "emp/web/Color.hpp"

namespace evoke {
  
  struct World {
    const emp::Size2D size = {512,512};  // Size of the world.
    double repro_prob = 0.0;             // Chance of org to replicate each update.
    double drift = 0.0;                  // Amount of Brownian motion.
    double org_radius = 1.0;             // How big should each cell get?
    size_t max_link_count = 3;           // How many links can an org have?

    // Extra information we want associated with each body.
    struct BodyInfo : public emp::PhysicsBody {
      size_t repro_id = emp::MAX_SIZE_T;  // Which ID is this body currently gestating?

      BodyInfo(emp::Circle circle = emp::Circle{1.0},
               emp::Color color = emp::Palette::BLACK)
        : PhysicsBody(circle, color) {}
    };
    using physics_t = emp::Physics2D<BodyInfo>;

    emp::Config config{};
    physics_t physics;
    emp::Random random;    

    World() : physics(size), random(1) {}

    void Init() {
      // Initialize an organism in the middle of the world.
      const emp::Point start_pos(size.Midpoint());
      physics.AddBody(emp::Circle(start_pos, org_radius));
    }

    void Reset() {
      // Clear everything currently in the world.
      physics.Clear();

      // Re-initialize a new organism in the middle of the world.
      Init();
    }

    /// Update the velocity of all bodies based on Brownian motion.
    void Update_Brownian() {
      if (drift == 0.0) return;  // If there's no brownian motion, skip this step.
      physics.ForEachBody([this](BodyInfo & body){
        const double drift_rad = random.GetDouble() * 2.0 * emp::PI;
        const emp::Angle drift_angle(drift_rad);
        body.IncSpeed(drift_angle.GetPoint(drift));
      });
    }

    emp::Point FindOffspringPos(const BodyInfo & body) {
      emp::Angle repro_angle(random.GetDouble(2.0 * emp::PI));
      emp::Point offspring_pos = body.GetCenter() + repro_angle.GetPoint();
      return physics.GetSize().Contains(offspring_pos) ? offspring_pos : FindOffspringPos(body);
    }

    void Update_Replicate() {
      // Loop through all bodies to see which ones should replicate.
      physics.ForEachBody([this](BodyInfo & body) {
        DEBUG_STACK();
        if (!body.IsActive()) return;

        // Bodies cannot reproduce IF:
        // * They are already reproducing
        // * They are attached to too many bodies.
        if (body.repro_id != emp::MAX_SIZE_T || body.GetLinkCount() >= max_link_count) return;

        if (random.P(repro_prob) || (physics.NumBodies() < 4 && random.P(repro_prob*3.0))) {
          emp::Point offspring_pos = FindOffspringPos(body);

          emp::Color new_color = body.GetColor();  // Default to parent color.
          const double target_radius = body.GetTargetRadius();
          if (random.P(0.1)) new_color = emp::HSL(random.GetDouble(360.0), 100.0, 50.0);
          size_t off_id = physics.AddBody(emp::Circle{offspring_pos, 2}, new_color);
          physics.GetBody(off_id).SetTargetRadius(target_radius); // Offspring target = parent target.
        }
      });
    }

    void Update() {
      DEBUG_STACK();
      // Take a single timestep on the world physics.
      physics.Update();
      Update_Brownian();
      Update_Replicate();
    }
  };

}

#endif
