import subprocess
import numpy as np
from math import comb
import matplotlib.pyplot as plt
import seaborn as sns
import json
import time
from scipy.stats import binom, chi2

# Configuration
num_cards = input("Enter the number of cards: ")
num_attributes = input("Enter the number of attributes: ")
num_tables = input("Enter the number of tables: ")
cpp_executable = input("Enter the path to your compiled C++ program: ")


def run_cpp_program(num_cards, num_attributes, num_tables, cpp_executable, output_file="data.txt"):
    """Runs the C++ executable with the required inputs
    """
    
    input_str = f"{num_cards}\n{num_attributes}\n{num_tables}\nn\n"
    
    result = subprocess.run(
        [cpp_executable],
        input=input_str,
        text=True,
        capture_output=True
    )
    
    if result.returncode != 0:
        raise RuntimeError(f"Error running C++ program: {result.stderr}")

    # Wait a bit to ensure the file is written
    time.sleep(0.5)

    return parse_data_file(output_file)

def parse_data_file(file_path):
    """Reads the output file and extracts the number of SETs from each table."""
    with open(file_path, "r") as file:
        lines = file.readlines()

    set_counts = []
    prob_no_sets = None
    avg_sets = None

    set_counts_start = lines.index("# Number of SETs in each Table\n") + 2
    set_counts = list(map(int, lines[set_counts_start].strip().split()))


    for i, line in enumerate(lines):
        if line.startswith("# Probability of no SETs:"):
            prob_no_sets = float(line.split(":")[1].strip().replace("%", ""))
        elif line.startswith("# Average number of SETs:"):
            avg_sets = float(line.split(":")[1].strip())

    return set_counts, prob_no_sets, avg_sets


# Dictionary to store results
results = {}

for num_cards in range(1, int(num_cards)+1):  # Loop from 1 to the end
    print(f"Running for {num_cards} cards...")
    set_counts, prob_no_sets, avg_sets = run_cpp_program(num_cards, num_attributes, num_tables, cpp_executable)
    results[num_cards] = {
        "set_counts": set_counts,
        "prob_no_sets": prob_no_sets,
        "avg_sets": avg_sets
    }

# Optional: Save results to a JSON file
with open("set_results.json", "w") as f:
    json.dump(results, f, indent=4)

print("All simulations completed!")

# Load the saved JSON data
with open("set_results.json", "r") as f:
    results = json.load(f)


def plot_histogram(num_cards, num_attributes, num_tables):
    """Plots a histogram of the number of SETs for a given number of cards,
    with an overlaid Binomial fit.
    """
    if str(num_cards) not in results:
        print(f"No data found for {num_cards} cards.")
        return

    set_counts = results[str(num_cards)]["set_counts"]
    max_sets = max(set_counts)

    # Binomial setup
    n_combinations = comb(num_cards, 3)  # Will act as the "n"
    mean_sets = np.mean(set_counts)
    p_est = mean_sets / n_combinations # Estimate of the probability of a SET
    x_vals = np.arange(0, max_sets + 1)
    binomial_probs = binom.pmf(x_vals, n_combinations, p_est)

    # Plot the histogram of your data, overlaying the binomial fit
    plt.figure(figsize=(8, 5))
    plt.hist(set_counts, bins=range(max_sets + 1), align='left', edgecolor="black", alpha=0.7, density=True, label="Data")
    plt.plot(x_vals, binomial_probs, 'ro-', label=f'Binomial Fit (n={n_combinations}, p={p_est:.4f})')

    plt.xlabel("Number of SETs")
    plt.ylabel("Probability")
    plt.title(f"Histogram of SETs for {num_cards} cards, with {num_attributes} attributes, {num_tables} tables")
    plt.xticks(range(max_sets + 1))
    plt.grid(axis="y", linestyle="--", alpha=0.7)
    plt.legend()
    plt.savefig(f"histogram{num_cards}cards.png")
    plt.show()

    # -- Chi-Square Goodness-of-Fit Test --
    observed_counts, _ = np.histogram(set_counts, bins=range(max_sets + 2))
    expected_counts = binomial_probs * num_tables
    chi2_stat = 0.0
    for k in range(len(observed_counts)):
        if expected_counts[k] > 0:
            chi2_stat += (observed_counts[k] - expected_counts[k])**2 / expected_counts[k]
    dof = (max_sets + 1) - 1 - 1  # = max_sets - 1
    p_value = 1 - chi2.cdf(chi2_stat, dof)

    print("=== Chi-Square Goodness-of-Fit Test ===")
    print(f"Chi-square statistic: {chi2_stat:.4f}")
    print(f"Degrees of freedom:  {dof}")
    print(f"p-value:            {p_value:.4g}")
    print("========================================")

def plot_avg_sets(num_attributes, num_tables):
    """Plots the average number of SETs vs. the number of cards
    """
    num_cards = sorted(int(k) for k in results.keys())
    avg_sets = [results[str(n)]["avg_sets"] for n in num_cards]

    plt.figure(figsize=(8, 5))
    plt.scatter(num_cards, avg_sets, marker="o", linestyle="-", color="b")
    plt.xlabel("Number of cards")
    plt.ylabel("Average number of SETs")
    plt.title(f"Average number of SETs vs. number of cards, with {num_attributes} attributes, {num_tables} tables")
    plt.grid(True, linestyle="--", alpha=0.7)
    plt.savefig("avg_sets.png")
    plt.show()

def plot_prob_set(num_attributes, num_tables):
    """Plots the probability of at least one SET vs. the number of cards
    """
    num_cards = sorted(int(k) for k in results.keys())
    prob_set = [100 - results[str(n)]["prob_no_sets"] for n in num_cards]

    plt.figure(figsize=(8, 5))
    plt.scatter(num_cards, prob_set, marker="o", linestyle="-", color="r")
    plt.xlabel("Number of cards")
    plt.ylabel("Probability of at least one SET (%)")
    plt.title(f"Probability of at least one SET vs. number of cards, with {num_attributes} attributes, {num_tables} tables")
    plt.grid(True, linestyle="--", alpha=0.7)
    plt.savefig("prob_set.png")
    plt.show()

# Plot the three graphs:
plot_histogram(num_cards, num_attributes, num_tables)
plot_avg_sets(num_attributes, num_tables)
plot_prob_set(num_attributes, num_tables)