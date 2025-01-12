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
  void RenderChain();
  void Update() override;
  glm::vec2 target_position_{};

 private:
  glm::vec2 velocity_{};
  glm::vec2 pull_velocity_{};
  bool should_pull_{false};
  std::vector<uint32_t> first_damaged_;
  std::vector<uint32_t> second_damaged_;
  unit::Devil *father_unit_{};
  std::vector<std::pair<uint32_t, float>> displacement_units_;
  // Units that are being displaced by the chain. The first value is the id of
  // the unit, the second one is the total displacement length. The displacement
  // finished when the second value is larger than 2.0f
};
}  // namespace battle_game::bullet
