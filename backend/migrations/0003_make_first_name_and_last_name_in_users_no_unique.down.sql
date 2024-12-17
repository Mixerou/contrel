WITH grouped_users AS (SELECT id,
                              first_name,
                              row_number() OVER (PARTITION BY first_name ORDER BY created_at) AS row_number
                       FROM users)
UPDATE users
SET first_name = grouped_users.first_name || grouped_users.row_number - 1
FROM grouped_users
WHERE grouped_users.row_number > 1
  AND users.id = grouped_users.id;

WITH grouped_users AS (SELECT id,
                              last_name,
                              row_number() OVER (PARTITION BY last_name ORDER BY created_at) AS row_number
                       FROM users)
UPDATE users
SET last_name = grouped_users.last_name || grouped_users.row_number - 1
FROM grouped_users
WHERE grouped_users.row_number > 1
  AND users.id = grouped_users.id;

ALTER TABLE users
    ADD CONSTRAINT users_first_name_key
        UNIQUE (first_name);

ALTER TABLE users
    ADD CONSTRAINT users_last_name_key
        UNIQUE (last_name);
