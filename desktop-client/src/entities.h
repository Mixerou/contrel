//
// Created by Mixerou on 18.12.2024.
//

#ifndef DESKTOP_CLIENT_ENTITIES_H
#define DESKTOP_CLIENT_ENTITIES_H

namespace entities {
typedef int64_t user_id_t;
typedef int64_t hotel_id_t;

struct User {
  user_id_t id;
  std::string email;
  std::string first_name;
  std::string last_name;
  int64_t created_at;

  MSGPACK_DEFINE(id, email, first_name, last_name, created_at);
};

struct Hotel {
  user_id_t id;
  std::string name;
  user_id_t owner_id;
  int64_t created_at;

  MSGPACK_DEFINE(id, name, owner_id, created_at);
};
}  // namespace entities

#endif  // DESKTOP_CLIENT_ENTITIES_H
