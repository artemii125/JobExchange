-- [AddApplicant]
INSERT INTO applicants (full_name, birth_date, phone, email, specialty, experience_years) 
VALUES (:name, :birth, :phone, :email, :spec, :exp);

-- [CheckApplicantExists]
SELECT COUNT(*) FROM applicants WHERE full_name = :name OR phone = :phone OR email = :email;

-- [RemoveApplicant]
DELETE FROM applicants WHERE id = :id;

-- [AddCompany]
INSERT INTO companies (name, inn, address, phone, contact_person) 
VALUES (:name, :inn, :address, :phone, :contact);

-- [CheckCompanyExists]
SELECT COUNT(*) FROM companies WHERE inn = :inn OR name = :name OR address = :address OR phone = :phone;

-- [RemoveCompany]
DELETE FROM companies WHERE id = :id;

-- [GetAllCompanies]
SELECT id, name, inn, address, phone, contact_person FROM companies;

-- [AddVacancy]
INSERT INTO vacancies (company_id, specialty, salary, status) 
VALUES (:company_id, :spec, :salary, 'Активна');

-- [CheckVacancyExists]
SELECT COUNT(*) FROM vacancies 
WHERE company_id = :company_id AND specialty = :spec;

-- [RemoveVacancy]
DELETE FROM vacancies WHERE id = :id;

-- [GetAllVacancies]
SELECT v.id, c.name as company_name, v.specialty, v.salary, v.status 
FROM vacancies v
JOIN companies c ON v.company_id = c.id;

-- [AddApplication]
INSERT INTO applications (applicant_id, vacancy_id, status) 
VALUES (:aid, :vid, 'Ожидание');

-- [UpdateApplicationStatus]
UPDATE applications SET status = :status WHERE id = :id;

-- [GetAllApplications]
SELECT app.id, app.applicant_id, a.full_name, v.specialty, app.status, app.application_date
FROM applications app
JOIN applicants a ON app.applicant_id = a.id
JOIN vacancies v ON app.vacancy_id = v.id;

-- [RemoveApplication]
DELETE FROM applications WHERE id = :id;

-- [CheckApplicationExists]
SELECT COUNT(*) FROM applications 
WHERE applicant_id = :a_id AND vacancy_id = :v_id AND status = 'Ожидание';

-- [GetAllVacancies]
SELECT v.id, c.name, v.specialty, v.salary, v.posted_date 
FROM vacancies v 
JOIN companies c ON v.company_id = c.id;

-- [LoginUser]
SELECT id, login, role_id FROM users WHERE login = :login AND password_hash = :password;