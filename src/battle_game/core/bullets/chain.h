#pragma once
#include "battle_game/core/bullet.h"
#include "battle_game/core/unit.h"
#include "battle_game/core/units/devil_tank.h"

namespace battle_game::unit {
class Devil;
}

namespace battle_game::bullet {
class Chain : public Bullet {
 public:
  Chain(GameCore *core,
        uint32_t id,
        uint32_t unit_id,
        uint32_t player_id,
        glm::vec2 position,
        float rotation,
        float damage_scale,
        glm::vec2 velocity,
        glm::vec2 target_position,
        unit::Devil *father_unit);
  ~Chain() override;
  void Render() override;
  void Update() override;
  glm::vec2 target_position_{};

 private:
  glm::vec2 velocity_{};
  glm::vec2 pull_velocity_{};
  bool should_pull_{false};
  bool first_damaged_{false};
  bool second_damaged_{false};
  unit::Devil *father_unit_{};
};
}  // namespace battle_game::bullet
