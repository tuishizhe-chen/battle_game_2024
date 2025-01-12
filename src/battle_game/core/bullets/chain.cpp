#include "battle_game/core/bullets/chain.h"

#include "battle_game/core/game_core.h"
#include "battle_game/core/particles/particles.h"
#include "battle_game/core/unit.h"
#include "battle_game/core/units/devil_tank.h"
#include "battle_game/graphics/graphics.h"

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
  first_damaged_ = {};
  second_damaged_ = {};
  displacement_units_ = {};
}

bool id_in_vector(std::vector<uint32_t> &vec, uint32_t id) {
  for (auto &i : vec) {
    if (i == id) {
      return true;
    }
  }
  return false;
}

void Chain::Render() {
  SetTransformation(position_,
                    atan2(velocity_.y, velocity_.x) - glm::radians(90.0f),
                    glm::vec2{0.2f});
  SetColor(game_core_->GetPlayerColor(player_id_));
  SetTexture(BATTLE_GAME_ASSETS_DIR "textures/particle3.png");
  DrawModel(father_unit_->chain_head_model_index);
}

void Chain::RenderChain() {
  auto current_head_position_ = position_;
  auto devil_position_ = father_unit_->GetPosition();
  battle_game::SetTransformation(
      current_head_position_,
      atan2(velocity_.y, velocity_.x) - glm::radians(90.0f));
  battle_game::SetTexture(0);
  battle_game::SetColor(
      glm::vec4{184.0f / 256.0f, 184.0f / 256.0f, 184.0f / 256.0f, 1.0f});
  battle_game::DrawModel(father_unit_->chain_head_model_index);
  while (length(current_head_position_ - devil_position_) > 0.3f) {
    auto to_devil_rotation = atan2((devil_position_ - current_head_position_).y,
                                   (devil_position_ - current_head_position_)
                                       .x);  // The direction head to devil
    battle_game::SetTransformation(current_head_position_,
                                   to_devil_rotation - glm::radians(90.0f));
    battle_game::SetTexture(0);
    battle_game::SetColor(
        glm::vec4{206.0f / 256.0f, 206.0f / 256.0f, 206.0f / 256.0f, 1.0f});
    battle_game::DrawModel(father_unit_->chain_model_index);
    current_head_position_ += Rotate(glm::vec2{0.25f, 0.0f}, to_devil_rotation);
  }
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
  // Check if the chain hit any unit
  if (!should_pull_) {
    for (auto &unit : units) {
      if (unit.first == unit_id_) {
        continue;
      }
      if (unit.second->IsHit(position_)) {
        bool this_unit_first_damaged = id_in_vector(first_damaged_, unit.first);
        if (this_unit_first_damaged) {
          continue;
        }
        game_core_->PushEventDealDamage(unit.first, id_, damage_scale_ * 10.0f);
        first_damaged_.push_back(unit.first);
      }
    }
  }
  // Check if the chain have reached the target position
  if (!should_pull_ && glm::length(position_ - target_position_) < 0.2f) {
    should_pull_ = true;
    father_unit_->IsPulling = true;
    // rotate the father unit to the position of the chain
    uint32_t father_id_ = father_unit_->GetId();
    float target_rotation_ =
        std::atan2((position_ - father_unit_->GetPosition()).y,
                   (position_ - father_unit_->GetPosition()).x);
    father_unit_->SetRotation(target_rotation_ - glm::radians(90.0f));
  }
  // Check if the father unit has reached the target position
  if (should_pull_ && father_unit_->IsHit(position_)) {
    should_die = true;
    father_unit_->IsPulling = false;
  }
  // The pull effect, check whether damage other units
  if (should_pull_ && !(father_unit_->IsHit(position_))) {
    auto p1 = father_unit_->GetPosition();
    auto p2 = position_;
    auto rotate = p2 - p1;
    auto rotate_angel = std::atan2(rotate.y, rotate.x);
    pull_velocity_ = Rotate(glm::vec2{20.0f, 0.0f}, rotate_angel);
    father_unit_->SetPosition(father_unit_->GetPosition() +
                              pull_velocity_ * kSecondPerTick);
    // Check if the father unit hit any unit
    auto father_pos = father_unit_->GetPosition();
    for (auto &unit : units) {
      if (unit.first == father_unit_->GetId()) {
        continue;
      }
      if (unit.second->IsHit(father_pos)) {
        if (!id_in_vector(second_damaged_, unit.first)) {
          // double damage if the unit is marked by electric effect
          if (id_in_vector(father_unit_->Electric_Effect_id, unit.first)) {
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
            // mark the unit with electric effect
          }
          // displace the unit, push into displacement_units_
          unit.second->SetPosition(unit.second->GetPosition() +
                                   pull_velocity_ * kSecondPerTick);
          displacement_units_.push_back(
              std::make_pair(std::move(unit.first),
                             glm::length(pull_velocity_ * kSecondPerTick)));
          second_damaged_.push_back(unit.first);
        }
      }
    }
    // Check if the displayment finished
    for (auto &unit : units) {
      for (auto it = displacement_units_.begin();
           it != displacement_units_.end(); it++) {
        if (unit.first == it->first) {
          if (it->second > 2.0f) {
            // remove the unit from displacement_units_
            displacement_units_.erase(it);
          } else if (!game_core_->IsBlockedByObstacles(
                         unit.second->GetPosition() +
                         pull_velocity_ * kSecondPerTick)) {
            it->second += glm::length(pull_velocity_ * kSecondPerTick);
            unit.second->SetPosition(unit.second->GetPosition() +
                                     pull_velocity_ * kSecondPerTick);
          }
          break;
        }
      }
    }
  }

  RenderChain();

  if (should_die) {
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
