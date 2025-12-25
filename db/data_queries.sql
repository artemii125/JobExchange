-- Запросы для работы с данными

-- GetAllCompanies: Получить все компании
SELECT id, name FROM companies;

-- GetAllVacancies: Получить все вакансии с компаниями
SELECT v.id, c.name, v.specialty, v.salary, v.posted_date, v.status 
FROM vacancies v 
JOIN companies c ON v.company_id = c.id;

-- GetVacanciesByCompany: Получить вакансии конкретной компании
SELECT v.id, c.name, v.specialty, v.salary, v.posted_date, v.status 
FROM vacancies v 
JOIN companies c ON v.company_id = c.id 
WHERE c.id = ?;

-- GetAllApplications: Получить все заявки
SELECT a.id, a.applicant_id, ap.full_name, v.specialty, a.status, a.application_date 
FROM applications a 
JOIN applicants ap ON a.applicant_id = ap.id 
JOIN vacancies v ON a.vacancy_id = v.id;

-- GetApplicationsByApplicant: Получить заявки соискателя
SELECT a.id, a.applicant_id, ap.full_name, v.specialty, a.status, a.application_date 
FROM applications a 
JOIN applicants ap ON a.applicant_id = ap.id 
JOIN vacancies v ON a.vacancy_id = v.id 
WHERE a.applicant_id = ?;

-- GetApplicationsByCompany: Получить заявки на вакансии компании
SELECT a.id, a.applicant_id, ap.full_name, v.specialty, a.status, a.application_date 
FROM applications a 
JOIN applicants ap ON a.applicant_id = ap.id 
JOIN vacancies v ON a.vacancy_id = v.id 
JOIN companies c ON v.company_id = c.id 
WHERE c.id = ?;

-- ExportTable: Экспорт таблицы
SELECT * FROM ?;

-- UpdateApplicationStatus: Обновить статус заявки
UPDATE applications SET status = ? WHERE id = ?;
