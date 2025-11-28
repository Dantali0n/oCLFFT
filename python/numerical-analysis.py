#!/usr/bin/env python3
"""
plot_fft_errors.py
------------------

Read a collection of CSV files that contain the error norms of different FFT
implementations and visualise them together with the theoretical upper bound

    bound(N) = c * eps * log2(N)

* The bound is plotted **only for L1, L2 and Linf** (not for RMS).
* The x‑axis shows the exact sample sizes `N` that appear in the CSV files
  (ticks are placed at those values).
* All four norms are shown on a 2×2 log‑log grid.

Usage
-----

    python plot_fft_errors.py <folder-with-csv> [--c <float>]
"""

import argparse
import glob
import os
import sys

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter


# ----------------------------------------------------------------------
# Helper functions
# ----------------------------------------------------------------------
def read_csv_file(path: str) -> pd.DataFrame:
    """Load a CSV file that must contain the columns N, L1, L2, Linf, RMS."""
    try:
        df = pd.read_csv(path, comment="#")
        # Normalise column names
        df = df.rename(columns=lambda c: c.strip())
        required = {"n", "l1", "l2", "linf", "rms"}
        missing = required - set(col.strip().lower() for col in df.columns)
        if missing:
            raise ValueError(f"Missing columns {missing}")
        return df
    except Exception as exc:
        print(f"[ERROR] Could not read '{path}': {exc}", file=sys.stderr)
        raise


def theoretical_bound_l2(N: np.ndarray, c: float) -> np.ndarray:
    """c * eps * log2(N)  (eps = double‑precision machine epsilon)."""
    eps = np.finfo(np.float32).eps
    return c * eps * np.log2(N)


def theoretical_bound_linf(N: np.ndarray) -> np.ndarray:
    """c * eps * log2(N)  (eps = double‑precision machine epsilon)."""
    eps = np.finfo(np.float32).eps
    return np.pow(2, np.log2(N)+1) * (2*np.log2(N)+1) * np.sqrt(2) * eps


def nice_label(csv_path: str) -> str:
    """Turn a CSV filename into a short, printable label."""
    base = os.path.basename(csv_path)
    name, _ = os.path.splitext(base)
    return name


# ----------------------------------------------------------------------
# Plotting routine
# ----------------------------------------------------------------------
def plot_error_norms(csv_paths, c_factor: float):
    # ------------------------------------------------------------------
    # Figure layout (2 × 2)
    # ------------------------------------------------------------------
    fig, axes = plt.subplots(2, 2, figsize=(12, 9), sharex=True)
    ax_dict = {
        "L1": axes[0, 0],
        "L2": axes[0, 1],
        "Linf": axes[1, 0],
        "RMS": axes[1, 1],
    }

    # ------------------------------------------------------------------
    # Collect *all* distinct N values (for the x‑axis tick marks)
    # ------------------------------------------------------------------
    all_N = np.unique(
        np.concatenate(
            [read_csv_file(p)["N"].values.astype(np.float64) for p in csv_paths]
        )
    )
    # Use a geometric spacing for the bound line later
    N_fine = np.geomspace(all_N.min(), all_N.max(), num=200)

    # ------------------------------------------------------------------
    # Plot the data of every implementation
    # ------------------------------------------------------------------
    for path in csv_paths:
        label = nice_label(path)
        df = read_csv_file(path).sort_values("N")
        N = df["N"].values.astype(np.float64)

        for norm_name, ax in ax_dict.items():
            y = df[norm_name].values.astype(np.float64)
            ax.loglog(
                N,
                y,
                marker="o",
                linestyle="-",
                label=label,
                linewidth=1.2,
                markersize=4,
            )

    # ------------------------------------------------------------------
    # Plot the theoretical bound – ONLY on L1, L2 and Linf
    # ------------------------------------------------------------------
    bound_vals = {
        "L2": theoretical_bound_l2(N_fine, c_factor),
        "Linf": theoretical_bound_linf(N_fine)
    }
    bound_labels = {
        "L2": f"c·ε·log₂N, c={c_factor}",
        "Linf": "2^(2log₂N+1)·(2log₂N+1)·√2·ε"
    }

    for norm_name in ("L1", "L2", "Linf"):
        if norm_name not in bound_vals:
            continue
        ax = ax_dict[norm_name]
        ax.loglog(
            N_fine,
            bound_vals[norm_name],
            color="black",
            linestyle="--",
            linewidth=2,
            label=f"Theoretical bound ({bound_labels[norm_name]})",

        )

    # ------------------------------------------------------------------
    # Axis cosmetics
    # ------------------------------------------------------------------
    # Titles
    ax_dict["L1"].set_title(r"$\ell_1$ norm")
    ax_dict["L2"].set_title(r"$\ell_2$ norm")
    ax_dict["Linf"].set_title(r"$\ell_{\infty}$ norm")
    ax_dict["RMS"].set_title(r"RMS norm")

    # Shared labels
    for ax in axes[1, :]:
        ax.set_xlabel("Number of samples $N$")
    for norm_name, ax in ax_dict.items():
        ax.set_ylabel(norm_name)

    # Show the *exact* N values as tick marks on the x‑axis
    plain_int_formatter = FuncFormatter(lambda x, pos: f"{int(x):d}" if x > 0 else "")
    for ax in axes[1, :]:
        ax.set_xscale("log")
        ax.set_xticks(all_N, minor=False)
        ax.get_xaxis().set_major_formatter(plain_int_formatter)
        ax.tick_params(axis="x", rotation=45)

    # Grid & legend
    for ax in ax_dict.values():
        ax.grid(which="both", linestyle=":", linewidth=0.5)
        ax.legend(fontsize="small", loc="best")

    fig.suptitle("FFT error norms vs. problem size", fontsize=16, y=0.97)
    fig.tight_layout(rect=[0, 0.03, 1, 0.95])

    plt.show()


# ----------------------------------------------------------------------
# CLI entry point
# ----------------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description="Plot L1/L2/Linf/RMS error norms for FFT implementations."
    )
    parser.add_argument(
        "folder",
        help="Path to the folder that contains the CSV files (one per implementation).",
    )
    parser.add_argument(
        "--c",
        type=float,
        default=1.0,
        help="Constant multiplying the theoretical bound (default: 1.0).",
    )
    args = parser.parse_args()

    folder = os.path.abspath(args.folder)
    if not os.path.isdir(folder):
        print(f"[ERROR] '{folder}' is not a directory.", file=sys.stderr)
        sys.exit(1)

    csv_paths = sorted(glob.glob(os.path.join(folder, "*.csv")))
    if not csv_paths:
        print(f"[ERROR] No CSV files found in '{folder}'.", file=sys.stderr)
        sys.exit(1)

    print(f"Found {len(csv_paths)} CSV file(s):")
    for p in csv_paths:
        print(f"  - {os.path.basename(p)}")

    plot_error_norms(csv_paths, c_factor=args.c)


if __name__ == "__main__":
    main()