"""Generate basic EDA charts for the University Rankings dataset.

Usage:
    python analysis/generate_charts.py
"""

from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data" / "powerbi_university_rankings_dataset.csv"
OUT = ROOT / "visuals"
OUT.mkdir(parents=True, exist_ok=True)

df = pd.read_csv(DATA)

# 1) Score distribution
plt.figure()
plt.hist(df["Score"].dropna(), bins=15)
plt.title("Distribution of University Scores")
plt.xlabel("Score")
plt.ylabel("Count")
plt.tight_layout()
plt.savefig(OUT / "score_distribution.png", dpi=200)
plt.close()

# 2) Top 10 countries by average score
country_stats = (df.groupby("Country")
                   .agg(avg_score=("Score","mean"), count=("Institution Name","count"))
                   .reset_index()
                   .sort_values(["avg_score","count"], ascending=[False, False])
                   .head(10))
plt.figure()
plt.bar(country_stats["Country"], country_stats["avg_score"])
plt.title("Top 10 Countries by Average University Score")
plt.xlabel("Country")
plt.ylabel("Average Score")
plt.xticks(rotation=45, ha="right")
plt.tight_layout()
plt.savefig(OUT / "top10_countries_avg_score.png", dpi=200)
plt.close()

# 3) Universities by continent
cont_counts = df["Continent"].value_counts()
plt.figure()
plt.bar(cont_counts.index.astype(str), cont_counts.values)
plt.title("Universities Count by Continent")
plt.xlabel("Continent")
plt.ylabel("Count")
plt.xticks(rotation=30, ha="right")
plt.tight_layout()
plt.savefig(OUT / "universities_by_continent.png", dpi=200)
plt.close()

# 4) Score vs Research Performance Rank
plt.figure()
plt.scatter(df["Research Performance Rank"], df["Score"])
plt.title("Score vs Research Performance Rank")
plt.xlabel("Research Performance Rank (lower is better)")
plt.ylabel("Score")
plt.tight_layout()
plt.savefig(OUT / "score_vs_research_rank.png", dpi=200)
plt.close()

print("Charts saved to:", OUT)
