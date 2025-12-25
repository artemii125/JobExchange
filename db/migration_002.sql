-- Миграция 002: Добавление каскадного удаления для applications

-- Удаляем старые внешние ключи
ALTER TABLE applications DROP CONSTRAINT IF EXISTS applications_applicant_id_fkey;
ALTER TABLE applications DROP CONSTRAINT IF EXISTS applications_vacancy_id_fkey;

-- Добавляем новые с каскадным удалением
ALTER TABLE applications 
ADD CONSTRAINT applications_applicant_id_fkey 
FOREIGN KEY (applicant_id) REFERENCES applicants(id) ON DELETE CASCADE;

ALTER TABLE applications 
ADD CONSTRAINT applications_vacancy_id_fkey 
FOREIGN KEY (vacancy_id) REFERENCES vacancies(id) ON DELETE CASCADE;
