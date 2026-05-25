"""
train.py
--------
Run AFTER data_collector.py has finished.
Trains Isolation Forest on baseline.csv and saves the model.

Usage:
    python3 train.py
"""

import pandas as pd
import numpy as np
import pickle, json, os

from sklearn.ensemble import IsolationForest
from sklearn.preprocessing import StandardScaler

CSV_PATH    = "data/baseline.csv"
MODEL_PATH  = "models/isolation_forest.pkl"
SCALER_PATH = "models/scaler.pkl"
STATS_PATH  = "models/baseline_stats.json"

os.makedirs("models", exist_ok=True)

# ── Load data ─────────────────────────────────────────────
print("Loading baseline data...")
df = pd.read_csv(CSV_PATH)
print("Rows loaded: {}".format(len(df)))

if len(df) < 60:
    print("WARNING: Less than 60 rows. Collect more data for better accuracy.")

features = df[["t1", "h1", "t2", "h2"]].values

# ── Scale features ────────────────────────────────────────
scaler = StandardScaler()
features_scaled = scaler.fit_transform(features)

# ── Train Isolation Forest ────────────────────────────────
print("Training Isolation Forest...")
iso_forest = IsolationForest(
    n_estimators=200,
    contamination=0.05,
    random_state=42
)
iso_forest.fit(features_scaled)

# Save model and scaler
with open(MODEL_PATH, "wb") as f:
    pickle.dump(iso_forest, f)

with open(SCALER_PATH, "wb") as f:
    pickle.dump(scaler, f)

print("Model saved to", MODEL_PATH)

# ── Save baseline stats for rate-of-change detector ──────
stats = {
    "t1_mean": float(df["t1"].mean()),
    "t1_std":  float(df["t1"].std()),
    "t2_mean": float(df["t2"].mean()),
    "t2_std":  float(df["t2"].std()),
    "h1_mean": float(df["h1"].mean()),
    "h2_mean": float(df["h2"].mean()),

    # 3-sigma thresholds = "critical" boundary
    "t1_critical": float(df["t1"].mean() + 3 * df["t1"].std()),
    "t2_critical": float(df["t2"].mean() + 3 * df["t2"].std()),
}

with open(STATS_PATH, "w") as f:
    json.dump(stats, f, indent=2)

print("Baseline stats saved:")
for k, v in stats.items():
    print("  {}: {:.3f}".format(k, v))

print("\nTraining complete. Run detector.py next.")