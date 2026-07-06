# Outtern — backend architecture proposal

R&D deliverable, Sprint 1 · Khun Htet Lin Aung · 15 June 2026 · v1.0 (draft)

Backend for a student–business internship marketplace. Stack: Node.js + Express, React + Vite frontend. This covers auth, authorisation, database, API, and data protection.

## Recommendations at a glance

| Area | Recommendation |
|------|----------------|
| Authentication | JWT — short-lived access token (in memory) + refresh token in httpOnly cookie. OAuth (Google/Microsoft) optional. |
| Authorisation | RBAC — 4 roles, checked in Express middleware, plus ownership checks. |
| Database | PostgreSQL + Prisma. |
| API | Versioned REST under `/api/v1`, one shared security pipeline. |
| Data protection | Argon2id passwords, TLS, encryption at rest, GDPR baseline. |

## Users

| Actor | Does |
|-------|------|
| Student | Profile, browse + apply to internships. |
| Business | Company profile (Y-tunnus), post internships, view applicants. |
| Admin | Verify profiles, run matching, moderate. |
| Super-admin | Manage admins, assign roles, audit logs. |

---

## 1. Authentication

### Options

| Approach | Pro | Con |
|----------|-----|-----|
| Session | Instant revoke, simple. | Needs shared store to scale; cookie-bound. |
| JWT | Stateless, scales, fits separate frontend. | Hard to revoke before expiry. |
| OAuth | Low friction, no password to store. | Provider dependency. |

### Recommended: JWT + refresh, OAuth optional

- **Access token** — JWT, ~15 min, holds user id + role, sent as `Authorization: Bearer`, kept in memory.
- **Refresh token** — ~7 days, httpOnly + Secure + SameSite=Strict cookie, only used at `/auth/refresh`, revocable server-side.

Revocation problem solved by the refresh token; the common path stays stateless. OAuth users get issued the same Outtern tokens after callback.

![JWT login and refresh flow](diag_auth.png)
*Figure 1 — JWT login and refresh flow*

### Notes
- Never use localStorage (XSS-readable). Memory + httpOnly cookie instead.
- Cookie → handle CSRF (SameSite=Strict + CSRF token on writes).
- Strong signing secret in env, never in repo. Set expiry; check issuer/audience.
- Rate-limit login/refresh. Rotate refresh tokens. Generic "invalid email or password".

---

## 2. Authorisation

RBAC: one role per user, role maps to permissions, checked before acting. ABAC is overkill here.

| Role | Permissions |
|------|-------------|
| Student | Read/edit own profile; browse internships; create/withdraw own applications. |
| Business | Read/edit own profile; create/edit/close own internships; view own applicants. |
| Admin | + read any profile, verify, run matches, moderate. |
| Super-admin | + assign roles, delete users, audit logs, config. |

![Role-to-permission mapping](diag_rbac.png)
*Figure 2 — Role-to-permission mapping*

Two layers:
- **Role-level** — middleware checks the JWT role can hit the route.
- **Ownership** — handler checks the user owns the record (stops the change-the-ID-in-the-URL bug).

```js
const permissions = {
  student:     ['profile:rw:own','internship:read','application:create'],
  business:    ['profile:rw:own','internship:rw:own','application:read:own'],
  admin:       ['profile:read:any','match:manage','user:moderate'],
  super_admin: ['role:assign','user:delete','audit:read'],
};

function requirePermission(perm) {
  return (req, res, next) => {
    if (!resolve(req.user.role).includes(perm))   // role from verified JWT
      return res.status(403).json({ error: 'Forbidden' });
    next();
  };
}

router.post('/internships', authenticate, requirePermission('internship:rw:own'), createInternship);
```

---

## 3. Database

### PostgreSQL vs MySQL

| | PostgreSQL | MySQL |
|--|-----------|-------|
| Integrity | Strict, rich constraints. | Solid, more lenient. |
| JSON | Native JSONB, indexable. | Weaker. |
| Full-text search | Built-in, good. | Weaker. |
| Hosting | Supabase, Neon, Railway, Render. | Widely hosted. |

**→ PostgreSQL + Prisma** (type-safe queries, managed migrations).

### Design: one identity, separate profiles

One `users` table for login + role. Role-specific data in separate profile tables, 1:1 to `users`. Keeps login uniform, avoids a wide half-empty table.

![ERD](diag_erd.png)
*Figure 3 — Core schema*

**users**

| Column | Type | Notes |
|--------|------|-------|
| id | uuid PK | |
| email | citext, unique | Case-insensitive. |
| password_hash | text | Argon2id; null for OAuth-only. |
| role | enum | student/business/admin/super_admin. |
| status | enum | pending/active/suspended. |
| email_verified | boolean | |
| created_at / updated_at / last_login_at | timestamptz | |

**student_profiles**

| Column | Type | Notes |
|--------|------|-------|
| id | uuid PK | |
| user_id | uuid FK, unique | → users |
| full_name, university, degree_programme | text | |
| study_year | int | |
| skills | jsonb | Indexed tags. |
| bio, availability | text | |
| cv_url, portfolio_url, linkedin_url | text | URLs to object storage, not files. |

**business_profiles**

| Column | Type | Notes |
|--------|------|-------|
| id | uuid PK | |
| user_id | uuid FK, unique | → users |
| company_name | text | |
| business_id | text | Y-tunnus, validated. |
| industry, website, contact_name, contact_phone, description | text | |
| logo_url | text | Object storage. |

**internships**

| Column | Type | Notes |
|--------|------|-------|
| id | uuid PK | |
| business_id | uuid FK | → business_profiles |
| title, description | text | Full-text searchable. |
| field | text | |
| hours | int | e.g. 150. |
| paid | boolean | |
| status | enum | draft/open/closed. |

Plus `applications` (student ↔ internship + status) and `refresh_tokens` (hashed token, user, expiry).

---

## 4. API

Versioned REST under `/api/v1`, by resource. Every request runs the same middleware pipeline.

![API pipeline](diag_api.png)
*Figure 4 — Layered architecture + request pipeline*

| Endpoint | Method | Purpose |
|----------|--------|---------|
| /auth/register | POST | Create account. |
| /auth/login | POST | Log in, issue tokens. |
| /auth/refresh | POST | New access token. |
| /auth/logout | POST | Revoke refresh token. |
| /students/me | GET/PATCH | Own student profile. |
| /students/:id | GET | A profile (admin / limited public). |
| /businesses/me | GET/PATCH | Own business profile. |
| /internships | GET/POST | List/search; create (business). |
| /internships/:id | GET/PATCH/DELETE | Read/edit/close (owner/admin). |
| /applications | GET/POST | List own; apply. |
| /admin/matches | GET/POST | Matching (admin). |

Conventions: plural nouns, no verbs in paths, `/me` for the caller, consistent JSON for success + errors, paging/filter/sort via query params.

Security: HTTPS + HSTS + Helmet · CORS allow-list (no wildcard) · Zod validation before logic · rate limiting · ORM-only queries (no SQL injection) · standard status codes, no stack traces to client.

---

## 5. Data protection

- **Passwords** — Argon2id (bcrypt fallback). Slow, memory-hard, auto-salted. Never plain text.
- **Encryption** — TLS in transit; DB encryption at rest; files in encrypted object storage (DB holds URL); secrets in env/secrets manager.

### GDPR

| Principle | Backend support |
|-----------|-----------------|
| Consent | Captured at registration, timestamped. |
| Minimisation | Only collect what matching needs. |
| Right of access | Export-own-data endpoint. |
| Right to erasure | Delete/anonymise endpoint. |
| Rectification | Profile-edit endpoints. |
| Storage limitation | Retention policy, prune stale data. |
| Security | Hashing, encryption, access control, logging. |
| Breach readiness | Logging/monitoring for 72-hour reporting. |

> GDPR is organisational too — Outtern also needs a privacy policy, processing record, and a named data contact. Raise with management.

---

## 6. Next steps

1. Lock the stack (Node/Express, PostgreSQL, Prisma, managed host — Supabase/Neon).
2. Scaffold Express + middleware pipeline + env config.
3. `users` table + register/login/refresh/logout.
4. Student + business profile tables + `/me` endpoints.
5. RBAC middleware + ownership checks, then internships + applications.
6. Security checklist + privacy notice draft for management.

**Assumptions:** separate React+Vite SPA · TypeScript backend · hosting open (managed Postgres + Node host) · v1, will evolve.