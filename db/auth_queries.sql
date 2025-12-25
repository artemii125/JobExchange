-- Запросы для авторизации и регистрации

-- LoginUser: Проверка логина и пароля
SELECT u.id, r.name, u.user_type, u.profile_id 
FROM users u 
JOIN roles r ON u.role_id = r.id 
WHERE u.login = ? AND u.password_hash = ?;

-- CheckUserExists: Проверка существования пользователя
SELECT id FROM users WHERE login = ?;

-- InsertUser: Создание нового пользователя
INSERT INTO users (login, password_hash, role_id, user_type) 
VALUES (?, ?, 2, ?) 
RETURNING id;

-- InsertCompany: Создание профиля компании
INSERT INTO companies (user_id, name, inn, address, phone, contact_person) 
VALUES (?, ?, ?, ?, ?, ?) 
RETURNING id;

-- InsertApplicant: Создание профиля соискателя
INSERT INTO applicants (user_id, full_name, birth_date, phone, email, specialty, experience_years, salary_expectation) 
VALUES (?, ?, ?, ?, ?, ?, ?, ?) 
RETURNING id;

-- UpdateUserProfile: Обновление profile_id в users
UPDATE users SET profile_id = ? WHERE id = ?;

-- GetUserRole: Получение роли пользователя
SELECT r.name 
FROM users u 
JOIN roles r ON u.role_id = r.id 
WHERE u.login = ? AND u.password_hash = ?;
