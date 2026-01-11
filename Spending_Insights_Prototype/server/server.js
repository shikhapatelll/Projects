import express from "express";
import cors from "cors";
import multer from "multer";
import Papa from "papaparse";
import initSqlJs from "sql.js";

const PORT = process.env.PORT || 5050;
const app = express();
app.use(cors());
app.use(express.json({ limit: "2mb" }));

const upload = multer({ storage: multer.memoryStorage(), limits: { fileSize: 5 * 1024 * 1024 } });

function titleCase(x) {
  return x.replace(/\w\S*/g, t => t[0].toUpperCase() + t.slice(1).toLowerCase());
}

function autoCategory(desc = "") {
  const s = desc.toLowerCase();
  const rules = [
    ["housing", ["rent", "landlord"]],
    ["groceries", ["grocery", "metro", "walmart", "loblaws", "superstore", "freshco"]],
    ["food & drink", ["starbucks", "tim hortons", "coffee", "restaurant", "pizza", "mcdonald"]],
    ["transport", ["uber", "lyft", "transit", "gas", "petro", "shell"]],
    ["subscriptions", ["netflix", "spotify", "prime", "membership", "gym"]],
    ["bills", ["hydro", "bell", "rogers", "internet", "phone", "utility"]],
    ["shopping", ["amazon", "ikea", "costco", "store", "winners"]],
    ["health", ["pharmacy", "clinic", "dental", "vision"]],
    ["income", ["payroll", "salary", "etransfer", "refund", "interest"]],
  ];
  for (const [cat, keys] of rules) {
    if (keys.some(k => s.includes(k))) return titleCase(cat);
  }
  return "Other";
}

function cleanAmount(raw) {
  if (raw == null) return NaN;
  let s = String(raw).trim();
  const negParen = /^\(.*\)$/.test(s);
  s = s.replace(/[$,]/g, "").replace(/[()]/g, "");
  const v = Number(s);
  if (!Number.isFinite(v)) return NaN;
  return negParen ? -Math.abs(v) : v;
}

function cleanDate(raw) {
  if (raw == null) return null;
  const s = String(raw).trim();
  const d = new Date(s);
  if (Number.isNaN(d.getTime())) return null;
  return d.toISOString().slice(0, 10);
}

let db = null;

async function initDb() {
  if (db) return db;
  const SQL = await initSqlJs({});
  db = new SQL.Database();
  db.run(`
    CREATE TABLE IF NOT EXISTS transactions (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      date TEXT NOT NULL,
      description TEXT NOT NULL,
      amount REAL NOT NULL,
      category TEXT NOT NULL
    );
  `);
  return db;
}

function runQuery(sql, params = []) {
  const stmt = db.prepare(sql);
  stmt.bind(params);
  const rows = [];
  while (stmt.step()) rows.push(stmt.getAsObject());
  stmt.free();
  return rows;
}

function scalar(sql, params = []) {
  const rows = runQuery(sql, params);
  if (!rows.length) return 0;
  const k = Object.keys(rows[0])[0];
  return rows[0][k] ?? 0;
}

function round2(x) {
  return Math.round(Number(x) * 100) / 100;
}

app.get("/api/health", async (_req, res) => {
  await initDb();
  res.json({ ok: true });
});

app.post("/api/upload", upload.single("file"), async (req, res) => {
  await initDb();
  if (!req.file) return res.status(400).json({ error: "Missing file" });

  const csvText = req.file.buffer.toString("utf8");
  const parsed = Papa.parse(csvText, { header: true, skipEmptyLines: true });

  if (parsed.errors?.length) {
    return res.status(400).json({ error: "CSV parse error", details: parsed.errors.slice(0, 3) });
  }

  const rows = parsed.data || [];
  let inserted = 0;
  let rejected = 0;
  const rejects = [];

  const insert = db.prepare("INSERT INTO transactions (date, description, amount, category) VALUES (?, ?, ?, ?)");
  db.run("BEGIN TRANSACTION;");
  try {
    for (const r of rows) {
      const keys = Object.fromEntries(Object.entries(r).map(([k, v]) => [String(k).toLowerCase().trim(), v]));
      const date = cleanDate(keys["date"]);
      const description = String(keys["description"] ?? keys["merchant"] ?? keys["name"] ?? "").trim();
      const amount = cleanAmount(keys["amount"]);
      const category = String(keys["category"] ?? "").trim() || autoCategory(description);

      if (!date || !description || !Number.isFinite(amount)) {
        rejected++;
        if (rejects.length < 10) rejects.push({ row: r, reason: "Invalid date/description/amount" });
        continue;
      }
      insert.run([date, description, amount, category]);
      inserted++;
    }
    db.run("COMMIT;");
  } catch (e) {
    db.run("ROLLBACK;");
    throw e;
  } finally {
    insert.free();
  }

  res.json({ inserted, rejected, sampleRejects: rejects });
});

app.get("/api/summary", async (_req, res) => {
  await initDb();
  const count = scalar("SELECT COUNT(*) AS n FROM transactions;");
  const totalIncome = scalar("SELECT COALESCE(SUM(amount),0) AS s FROM transactions WHERE amount > 0;");
  const totalSpend = scalar("SELECT COALESCE(SUM(amount),0) AS s FROM transactions WHERE amount < 0;");
  const net = totalIncome + totalSpend;

  const minDate = runQuery("SELECT MIN(date) as d FROM transactions;")[0]?.d ?? null;
  const maxDate = runQuery("SELECT MAX(date) as d FROM transactions;")[0]?.d ?? null;

  const topMerchants = runQuery(`
    SELECT description, ROUND(SUM(amount),2) AS total
    FROM transactions
    GROUP BY description
    ORDER BY ABS(total) DESC
    LIMIT 5;
  `);

  res.json({
    count,
    dateRange: { minDate, maxDate },
    totals: {
      income: round2(totalIncome),
      spending: round2(Math.abs(totalSpend)),
      net: round2(net),
    },
    topMerchants,
  });
});

app.get("/api/categories", async (_req, res) => {
  await initDb();
  const cats = runQuery(`
    SELECT category,
           ROUND(SUM(CASE WHEN amount < 0 THEN -amount ELSE 0 END), 2) AS spending,
           ROUND(SUM(CASE WHEN amount > 0 THEN amount ELSE 0 END), 2) AS income
    FROM transactions
    GROUP BY category
    ORDER BY spending DESC;
  `);
  res.json({ categories: cats });
});

app.get("/api/anomalies", async (req, res) => {
  await initDb();
  const zThresh = Number(req.query.z ?? 2.5);
  const spending = runQuery(`
    SELECT id, date, description, category, -amount AS spending
    FROM transactions
    WHERE amount < 0;
  `);

  const byCat = new Map();
  for (const t of spending) {
    if (!byCat.has(t.category)) byCat.set(t.category, []);
    byCat.get(t.category).push(t);
  }

  const anomalies = [];
  for (const [cat, items] of byCat.entries()) {
    const vals = items.map(x => x.spending);
    const mean = vals.reduce((a,b)=>a+b,0) / Math.max(vals.length,1);
    const variance = vals.reduce((a,b)=>a + (b-mean)*(b-mean),0) / Math.max(vals.length,1);
    const sd = Math.sqrt(variance) || 1e-9;

    for (const it of items) {
      const z = (it.spending - mean) / sd;
      if (z >= zThresh) {
        anomalies.push({ ...it, zScore: round2(z) });
      }
    }
  }

  anomalies.sort((a,b)=>b.zScore - a.zScore);
  res.json({ threshold: zThresh, anomalies: anomalies.slice(0, 50) });
});

app.get("/api/transactions", async (req, res) => {
  await initDb();
  const search = String(req.query.search ?? "").trim().toLowerCase();
  const limit = Math.max(1, Math.min(200, Number(req.query.limit ?? 50)));
  const offset = Math.max(0, Number(req.query.offset ?? 0));

  let where = "";
  let params = [];
  if (search) {
    where = "WHERE LOWER(description) LIKE ? OR LOWER(category) LIKE ?";
    const s = `%${search}%`;
    params = [s, s];
  }

  const rows = runQuery(
    `SELECT id, date, description, amount, category
     FROM transactions
     ${where}
     ORDER BY date DESC, id DESC
     LIMIT ? OFFSET ?;`,
    [...params, limit, offset]
  );
  res.json({ items: rows, limit, offset, search });
});

await initDb();
app.listen(PORT, () => console.log(`API running on http://localhost:${PORT}`));
