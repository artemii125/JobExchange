-- Миграция для добавления связи пользователей с профилями

-- Добавляем новые поля в users
ALTER TABLE users ADD COLUMN IF NOT EXISTS user_type VARCHAR(20);
ALTER TABLE users ADD COLUMN IF NOT EXISTS profile_id INT;

-- Добавляем user_id в companies и applicants
ALTER TABLE companies ADD COLUMN IF NOT EXISTS user_id INT UNIQUE REFERENCES users(id) ON DELETE CASCADE;
ALTER TABLE applicants ADD COLUMN IF NOT EXISTS user_id INT UNIQUE REFERENCES users(id) ON DELETE CASCADE;

-- Обновляем существующие данные (если есть)
-- Для существующих компаний и соискателей можно создать пользователей вручную или оставить NULL
