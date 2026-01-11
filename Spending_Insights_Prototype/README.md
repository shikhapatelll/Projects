# TD Invent–Style Prototype: Spending Insights (Web + API)

A lightweight full‑stack prototype that demonstrates rapid concept‑to‑prototype delivery for a banking-style use case:
upload transaction data → store it → analyze it → present insights and anomalies.

## What it does
1) Upload a CSV of transactions (sample included)
2) Backend parses + cleans data and stores it in an embedded SQLite database (via `sql.js`)
3) Frontend shows totals, category breakdown, top merchants, anomalies (outliers), and search.

## Tech stack
- Backend: Node.js, Express, `sql.js` (SQLite in JS/WASM), CSV parsing, basic analytics
- Frontend: Static HTML/CSS/JS + Chart.js (CDN)
- Docs: OpenAPI spec + revision checklist

## Quick start
### Backend
```bash
cd server
npm install
npm run dev
```

### Frontend
Open `client/index.html` in your browser.
The UI calls the API at `http://localhost:5050`.

### Upload sample data
Use the Upload box and choose: `sample_data/transactions_sample.csv`

## API
See: `docs/openapi.yaml`
