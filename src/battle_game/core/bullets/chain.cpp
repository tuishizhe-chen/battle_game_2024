#include "battle_game/core/bullets/chain.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"
#include "battle_game/core/unit.h"
#include "battle_game/core/units/devil_tank.h"

namespace battle_game::bullet {
Chain::Chain(GameCore *core,
             uint32_t id,
             uint32_t unit_id,
             uint32_t player_id,
             glm::vec2 position,
             float rotation,
             float damage_scale,
             glm::vec2 velocity,
             glm::vec2 target_position,
             unit::Devil *father_unit)
    : Bullet(core, id, unit_id, player_id, position, rotation, damage_scale),
      velocity_(velocity) {
  target_position_ = target_position;
  father_unit_ = father_unit;
  should_pull_ = false;
  first_damaged_ = false;
  second_damaged_ = false;
}

void Chain::Render() {
  SetTransformation(position_, rotation_, glm::vec2{0.1f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture(BATTLE_GAME_ASSETS_DIR "textures/particle3.png");
  DrawModel(0);
}

void Chain::Update() {
  if (!should_pull_) {
    position_ += velocity_ * kSecondPerTick;
  }
  bool should_die = false;
  if (game_core_->IsBlockedByObstacles(position_)) {
    should_die = true;
  }

  auto &units = game_core_->GetUnits();
  if (!first_damaged_ && !should_pull_) {
    for (auto &unit : units) {
      if (unit.first == unit_id_) {
        continue;
      }
      if (unit.second->IsHit(position_)) {
        game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
        first_damaged_ = true;
      }
    }
  }

  if (glm::length(position_ - target_position_) < 0.2f) {
    should_pull_ = true;
  }

  if (should_pull_ && father_unit_->IsHit(position_)) {
    should_die = true;
  }

  if (should_pull_ && !(father_unit_->IsHit(position_))) {
    auto p1 = father_unit_->GetPosition();
    auto p2 = position_;
    auto rotate = p2 - p1;
    auto rotate_angel = std::atan2(rotate.y, rotate.x);
    pull_velocity_ = Rotate(glm::vec2{20.0f, 0.0f}, rotate_angel);
    father_unit_->SetPosition(father_unit_->GetPosition() +
                              pull_velocity_ * kSecondPerTick);
    auto father_pos = father_unit_->GetPosition();
    if (!second_damaged_) {
      for (auto &unit : units) {
        if (unit.first == father_unit_->GetId()) {
          continue;
        }
        if (unit.second->IsHit(father_pos)) {
          bool electric_effect_exist = false;
          for (auto &id : father_unit_->Electric_Effect_id) {
            if (id == unit.first) {
              electric_effect_exist = true;
              break;
            }
          }
          if (electric_effect_exist) {
            game_core_->PushEventDealDamage(unit.first, id_,
                                            damage_scale_ * 20.0f);
            // remove the electric effect id
            for (auto it = father_unit_->Electric_Effect_id.begin();
                 it != father_unit_->Electric_Effect_id.end(); it++) {
              if (*it == unit.first) {
                father_unit_->Electric_Effect_id.erase(it);
                break;
              }
            }
          } else {
            game_core_->PushEventDealDamage(unit.first, id_,
                                            damage_scale_ * 10.0f);
            father_unit_->Electric_Effect_id.push_back(unit.first);
          }
          second_damaged_ = true;
        }
      }
    }
  }

  if (should_die) {
    father_unit_->Last_Chain = false;
    game_core_->PushEventRemoveBullet(id_);
  }
}

Chain::~Chain() {
  for (int i = 0; i < 5; i++) {
    game_core_->PushEventGenerateParticle<particle::Smoke>(
        position_, rotation_, game_core_->RandomInCircle() * 2.0f, 0.2f,
        glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
  }
}
}  // namespace battle_game::bullet
