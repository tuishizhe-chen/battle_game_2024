#pragma once
#include "battle_game/core/bullets/chain.h"
#include "battle_game/core/unit.h"

namespace battle_game::unit {
class Devil : public Unit {
 public:
  Devil(GameCore *game_core, uint32_t id, uint32_t player_id);
  void Render() override;
  void Update() override;
  [[nodiscard]] bool IsHit(glm::vec2 position) const override;
  std::vector<uint32_t> Electric_Effect_id;
  bool IsPulling;
  uint32_t chain_model_index{0xffffffffu};
  uint32_t chain_head_model_index{0xffffffffu};

 protected:
  void TankMove(float move_speed, float rotate_angular_speed);
  void TurretRotate();
  void BeginDash();
  void Dashing();
  void Fire();
  [[nodiscard]] const char *UnitName() const override;
  [[nodiscard]] const char *Author() const override;

  float turret_rotation_{0.0f};
  uint32_t fire_count_down_{0};
  uint32_t mine_count_down_{0};
  uint32_t dash_count_down_{0};
  bool IsDashing{false};

  std::vector<uint32_t> damaged_units_by_dash_;
};
}  // namespace battle_game::unit
