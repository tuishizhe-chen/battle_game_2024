#include "devil_tank.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/bullets/chain.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

namespace battle_game::unit {

namespace {
uint32_t tank_body_model_index = 0xffffffffu;
uint32_t tank_pulling_body_model_index = 0xffffffffu;
uint32_t tank_turret_model_index = 0xffffffffu;
uint32_t tank_pulling_turret_model_index = 0xffffffffu;
uint32_t tank_dashing_body_model_index = 0xffffffffu;
}  // namespace

Devil::Devil(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  Electric_Effect_id = {};
  damaged_units_by_dash_ = {};
  IsPulling = false;
  if (!~chain_head_model_index) {
    auto mer = AssetsManager::GetInstance();
    {
      /* Chain Head */
      std::vector<ObjectVertex> chain_head_vertices = {
          {{0.0f, 0.0f},
           {0.0f, 0.0f},
           {184.0f / 256.0f, 184.0f / 256.0f, 184.0f / 256.0f, 1.0f}},
          {{-0.2f, 0.3f},
           {0.0f, 0.0f},
           {184.0f / 256.0f, 184.0f / 256.0f, 184.0f / 256.0f, 1.0f}},
          {{0.2f, 0.3f},
           {0.0f, 0.0f},
           {184.0f / 256.0f, 184.0f / 256.0f, 184.0f / 256.0f, 1.0f}},
          {{0.0f, 0.8f},
           {0.0f, 0.0f},
           {184.0f / 256.0f, 184.0f / 256.0f, 184.0f / 256.0f, 1.0f}},
      };
      std::vector<uint32_t> chain_head_indices = {0, 1, 3, 0, 2, 3};
      chain_head_model_index =
          mer->RegisterModel(chain_head_vertices, chain_head_indices);
    }
  }
  if (!~chain_model_index) {
    auto mfr = AssetsManager::GetInstance();
    {
      /* Chain */
      std::vector<ObjectVertex> chain_vertices = {
          {{0.0f, 0.0f},
           {0.0f, 0.0f},
           {206.0f / 256.0f, 206.0f / 256.0f, 206.0f / 256.0f, 1.0f}},
          {{-0.1f, 0.4f},
           {0.0f, 0.0f},
           {206.0f / 256.0f, 206.0f / 256.0f, 206.0f / 256.0f, 1.0f}},
          {{0.1f, 0.4f},
           {0.0f, 0.0f},
           {206.0f / 256.0f, 206.0f / 256.0f, 206.0f / 256.0f, 1.0f}},
          {{0.0f, 0.2f},
           {0.0f, 0.0f},
           {206.0f / 256.0f, 206.0f / 256.0f, 206.0f / 256.0f, 1.0f}},
          {{0.1f, 0.5f},
           {0.0f, 0.0f},
           {206.0f / 256.0f, 206.0f / 256.0f, 206.0f / 256.0f, 1.0f}},
          {{-0.1f, 0.5f},
           {0.0f, 0.0f},
           {206.0f / 256.0f, 206.0f / 256.0f, 206.0f / 256.0f, 1.0f}}};
      std::vector<uint32_t> chain_indices = {0, 1, 3, 0, 2, 3,
                                             3, 4, 2, 3, 5, 1};
      chain_model_index = mfr->RegisterModel(chain_vertices, chain_indices);
    }
  }
  if (!~tank_body_model_index) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      std::vector<ObjectVertex> body_vertices = {
          {{-0.8f, 0.8f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
          {{-0.6f, -1.0f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
          {{0.8f, 0.8f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
          {{0.6f, -1.0f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
          // distinguish front and back
          {{0.7f, 0.8f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
          {{0.6f, 1.0f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
          {{0.5f, 0.8f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
          {{-0.5f, 0.8f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
          {{-0.6f, 1.0f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
          {{-0.7f, 0.8f},
           {0.0f, 0.0f},
           {148.0f / 256.0f, 10.0f / 256.0f, 45.0f / 256.0f, 1.0f}},
      };
      std::vector<uint32_t> body_indices = {0, 1, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9};
      tank_body_model_index = mgr->RegisterModel(body_vertices, body_indices);
    }

    {
      /* Tank Turret */
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.push_back({{sin_theta * 0.5f, cos_theta * 0.5f},
                                   {0.0f, 0.0f},
                                   {0.7f, 0.7f, 0.7f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      turret_vertices.push_back(
          {{0.0f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_indices.push_back(precision + 1 + 0);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);
      tank_turret_model_index =
          mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
  if (!~tank_pulling_body_model_index) {
    auto mhr = AssetsManager::GetInstance();
    {
      /* Tank Body */
      std::vector<ObjectVertex> body_vertices = {
          {{0.0f, 1.4f},
           {0.0f, 0.0f},
           {16.0f / 256.0f, 93.0f / 256.0f, 94.0f / 256.0f, 1.0f}},
          {{-1.0f, 0.0f},
           {0.0f, 0.0f},
           {16.0f / 256.0f, 93.0f / 256.0f, 94.0f / 256.0f, 1.0f}},
          {{1.0f, 0.0f},
           {0.0f, 0.0f},
           {16.0f / 256.0f, 93.0f / 256.0f, 94.0f / 256.0f, 1.0f}},
          {{0.0f, -1.0f},
           {0.0f, 0.0f},
           {16.0f / 256.0f, 93.0f / 256.0f, 94.0f / 256.0f, 1.0f}},
          {{0.4f, 0.4f},
           {0.0f, 0.0f},
           {16.0f / 256.0f, 93.0f / 256.0f, 94.0f / 256.0f, 1.0f}},
          {{-0.4f, 0.4f},
           {0.0f, 0.0f},
           {16.0f / 256.0f, 93.0f / 256.0f, 94.0f / 256.0f, 1.0f}},
          {{-0.4f, -0.4f},
           {0.0f, 0.0f},
           {16.0f / 256.0f, 93.0f / 256.0f, 94.0f / 256.0f, 1.0f}},
          {{0.4f, -0.4f},
           {0.0f, 0.0f},
           {16.0f / 256.0f, 93.0f / 256.0f, 94.0f / 256.0f, 1.0f}},
      };
      std::vector<uint32_t> body_indices = {0, 4, 5, 3, 6, 7, 1, 5, 6,
                                            2, 4, 7, 4, 5, 6, 4, 7, 6};
      tank_pulling_body_model_index =
          mhr->RegisterModel(body_vertices, body_indices);
    }
  }
  if (!~tank_dashing_body_model_index) {
    auto mir = AssetsManager::GetInstance();
    {
      /* Tank Body */
      std::vector<ObjectVertex> body_vertices = {
          {{0.8f, -0.2f},
           {0.0f, 0.0f},
           {59.0f / 256.0f, 160.0f / 256.0f, 193.0f / 256.0f, 1.0f}},
          {{-0.8f, -0.2f},
           {0.0f, 0.0f},
           {59.0f / 256.0f, 160.0f / 256.0f, 193.0f / 256.0f, 1.0f}},
          {{0.6f, -1.2f},
           {0.0f, 0.0f},
           {59.0f / 256.0f, 160.0f / 256.0f, 193.0f / 256.0f, 1.0f}},
          {{-0.6f, -1.2f},
           {0.0f, 0.0f},
           {59.0f / 256.0f, 160.0f / 256.0f, 193.0f / 256.0f, 1.0f}},
          {{0.4f, -0.6f},
           {0.0f, 0.0f},
           {59.0f / 256.0f, 160.0f / 256.0f, 193.0f / 256.0f, 1.0f}},
          {{-0.4f, -0.6f},
           {0.0f, 0.0f},
           {59.0f / 256.0f, 160.0f / 256.0f, 193.0f / 256.0f, 1.0f}},
          {{0.0f, 1.2f},
           {0.0f, 0.0f},
           {59.0f / 256.0f, 160.0f / 256.0f, 193.0f / 256.0f, 1.0f}},
      };
      std::vector<uint32_t> body_indices = {0, 2, 4, 1, 3, 5, 0, 1,
                                            4, 1, 4, 5, 0, 1, 6};
      tank_dashing_body_model_index =
          mir->RegisterModel(body_vertices, body_indices);
    }
  }
}

void Devil::Render() {
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  if (IsPulling && !IsDashing) {
    battle_game::DrawModel(tank_pulling_body_model_index);
    battle_game::SetRotation(turret_rotation_);
    battle_game::DrawModel(tank_turret_model_index);
  } else if (IsDashing) {
    battle_game::DrawModel(tank_dashing_body_model_index);
    battle_game::SetRotation(turret_rotation_);
    battle_game::DrawModel(tank_turret_model_index);
  } else {
    battle_game::DrawModel(tank_body_model_index);
    battle_game::SetRotation(turret_rotation_);
    battle_game::DrawModel(tank_turret_model_index);
  }
}

void Devil::Update() {
  TankMove(3.0f, glm::radians(180.0f));
  TurretRotate();
  Fire();
  BeginDash();
  Dashing();
}

void Devil::TankMove(float move_speed, float rotate_angular_speed) {
  if (!IsPulling && !IsDashing) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      glm::vec2 offset{0.0f};
      if (input_data.key_down[GLFW_KEY_W]) {
        offset.y += 1.5f;
      }
      if (input_data.key_down[GLFW_KEY_S]) {
        offset.y -= 1.5f;
      }
      float speed = move_speed * GetSpeedScale();
      offset *= kSecondPerTick * speed;
      auto new_position =
          position_ + glm::vec2{glm::rotate(glm::mat4{1.0f}, rotation_,
                                            glm::vec3{0.0f, 0.0f, 1.0f}) *
                                glm::vec4{offset, 0.0f, 0.0f}};
      if (!game_core_->IsBlockedByObstacles(new_position)) {
        game_core_->PushEventMoveUnit(id_, new_position);
      }
      float rotation_offset = 0.0f;
      if (input_data.key_down[GLFW_KEY_A]) {
        rotation_offset += 1.5f;
      }
      if (input_data.key_down[GLFW_KEY_D]) {
        rotation_offset -= 1.5f;
      }
      rotation_offset *=
          kSecondPerTick * rotate_angular_speed * GetSpeedScale();
      game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
    }
  }
}

void Devil::TurretRotate() {
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }
}

void Devil::BeginDash() {
  if (dash_count_down_ == 0 && !IsPulling) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.key_down[GLFW_KEY_F]) {
        IsDashing = true;
        dash_count_down_ = 10 * kTickPerSecond;  // Dash interval 10 second.
      }
    }
  }
  if (dash_count_down_) {
    dash_count_down_--;
  }
}

bool id_in_vector(std::vector<uint32_t> &vec, uint32_t id) {
  for (auto i : vec) {
    if (i == id) {
      return true;
    }
  }
  return false;
}

void Devil::Dashing() {
  if (IsDashing && !IsPulling) {
    auto player = game_core_->GetPlayer(player_id_);
    if (dash_count_down_ <= (9.4) * kTickPerSecond) {
      IsDashing = false;
    } else {
      auto dash_target = LocalToWorld(glm::vec2{0.0f, 12.0f} * kSecondPerTick);
      if (game_core_->IsBlockedByObstacles(dash_target)) {
        IsDashing = false;
      } else {
        game_core_->PushEventMoveUnit(id_, dash_target);
      }
      auto &units = game_core_->GetUnits();
      for (auto &unit : units) {
        if (unit.first == id_) {
          continue;
        }
        if (unit.second->IsHit(position_) &&
            !id_in_vector(damaged_units_by_dash_, unit.first)) {
          game_core_->PushEventDealDamage(unit.first, id_, 30.0f);
          damaged_units_by_dash_.push_back(unit.first);
        }
      }
    }
  }
}

void Devil::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        auto velocity = Rotate(glm::vec2{0.0f, 20.0f}, turret_rotation_);
        GenerateBullet<bullet::Chain>(
            position_ + Rotate({0.0f, 1.2f}, turret_rotation_),
            turret_rotation_, GetDamageScale(), velocity,
            input_data.mouse_cursor_position, this);
        fire_count_down_ = 3 * kTickPerSecond;  // Fire interval 3 second.
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool Devil::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *Devil::UnitName() const {
  return "Devil Tank";
}

const char *Devil::Author() const {
  return "TuiShiZhe";
}
}  // namespace battle_game::unit
