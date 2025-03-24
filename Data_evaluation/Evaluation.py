import subprocess
import numpy as np
from math import comb
import matplotlib.pyplot as plt
import json
import time
from scipy.stats import binom, chi2
from scipy.optimize import curve_fit

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

    plt.figure(figsize=(8, 5))
    hist_values, bin_edges, _ = plt.hist(set_counts, bins=range(max_sets + 2), align='left', edgecolor="black", alpha=0.7, density=True, label="Data")

    def gaussian(x, A, mu, sigma):
        return A * np.exp(-((x - mu) ** 2) / (2 * sigma ** 2))
    
    bin_centers = 0.5 * (bin_edges[:-1] + bin_edges[1:])  # midpoint of each bin
    p0 = [max(hist_values), np.mean(set_counts), np.std(set_counts)]
    popt, _ = curve_fit(gaussian, bin_centers, hist_values, p0=p0)
    A_opt, mu_opt, sigma_opt = popt

    x_fit = np.linspace(bin_edges[0], bin_edges[-1], 200)
    plt.plot( x_fit, gaussian(x_fit, A_opt, mu_opt, sigma_opt), 'r-', label=f'Gaussian Fit (A={A_opt:.2f}, μ={mu_opt:.2f}, σ={sigma_opt:.2f})')
    plt.xlabel("Number of SETs")
    plt.ylabel("Probability")
    plt.title(f"Histogram of SETs for {num_cards} cards, with {num_attributes} attributes, {num_tables} tables")
    plt.xticks(range(max_sets + 2))
    plt.grid(axis="y", linestyle="--", alpha=0.7)
    plt.legend()
    plt.savefig(f"histogram{num_cards}cards.png")
    plt.show()

    print("=== Gaussian Fit Results ===")
    print(f"Amplitude (A):  {A_opt:.4f}")
    print(f"Mean (μ):       {mu_opt:.4f}")
    print(f"Std Dev (σ):    {sigma_opt:.4f}")
    print("============================")
    print(bin_centers, bin_edges)
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