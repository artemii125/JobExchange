-- Роли
CREATE TABLE roles (
    id SERIAL PRIMARY KEY,
    name VARCHAR(20) UNIQUE NOT NULL
);

-- Пользователи
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    login VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role_id INT REFERENCES roles(id)
);

-- 1. КОМПАНИИ (Вкладка 3)
CREATE TABLE companies (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    inn VARCHAR(12) UNIQUE NOT NULL,
    address TEXT,
    phone VARCHAR(20),
    contact_person VARCHAR(100)
);

-- 2. СОИСКАТЕЛИ (Вкладка 1)
CREATE TABLE applicants (
    id SERIAL PRIMARY KEY,
    full_name VARCHAR(150) NOT NULL, -- ФИО
    birth_date DATE,
    phone VARCHAR(20),
    email VARCHAR(100),
    specialty VARCHAR(100),
    experience_years INT DEFAULT 0,
    salary_expectation DECIMAL(10, 2)
);

-- 3. ВАКАНСИИ (Вкладка 2)
CREATE TABLE vacancies (
    id SERIAL PRIMARY KEY,
    company_id INT REFERENCES companies(id) ON DELETE CASCADE,
    position VARCHAR(100) NOT NULL, -- Должность
    specialty VARCHAR(100),
    salary DECIMAL(10, 2),
    status VARCHAR(20) DEFAULT 'Активна', -- 'Активна', 'Закрыта'
    posted_date DATE DEFAULT CURRENT_DATE,
    description TEXT,
    requirements TEXT
);

-- 4. ЗАЯВКИ (Вкладка 4 - История)
CREATE TABLE applications (
    id SERIAL PRIMARY KEY,
    applicant_id INT REFERENCES applicants(id),
    vacancy_id INT REFERENCES vacancies(id),
    application_date DATE DEFAULT CURRENT_DATE,
    status VARCHAR(50) DEFAULT 'На рассмотрении'
);

-- Данные
INSERT INTO roles (name) VALUES ('admin'), ('user');
INSERT INTO users (login, password_hash, role_id) 
VALUES ('admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918', 1);