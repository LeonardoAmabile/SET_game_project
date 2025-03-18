import subprocess
import numpy as np
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import json
import time
from scipy.stats import binom

# Configuration
num_cards = input("Enter the number of cards: ")
num_attributes = input("Enter the number of attributes: ")
num_tables = input("Enter the number of tables: ")
cpp_executable = input("Enter the path to your compiled C++ program: ")


def run_cpp_program(num_cards, num_attributes, num_tables, cpp_executable, output_file="data.txt"):
    """Runs the C++ executable with the required inputs."""
    
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

for num_cards in range(1, int(num_cards)):  # Loop from 1 to the end
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


def plot_histogram(num_cards):
    """Plots a histogram of the number of sets for a given number of cards
    """
    if str(num_cards) not in results:
        print(f"No data found for {num_cards} cards.")
        return

    set_counts = results[str(num_cards)]["set_counts"]
    max_sets = max(set_counts)  # Maximum observed number of sets

    # Plot histogram
    plt.figure(figsize=(8, 5))
    plt.hist(set_counts, bins=range(max_sets + 2), align='left', edgecolor="black", alpha=0.7, label="Observed Data")
    plt.xlabel("Number of Sets")
    plt.ylabel("Frequency")
    plt.title(f"Histogram of sets for {num_cards} cards")
    plt.xticks(range(max_sets + 1))
    plt.legend()
    plt.grid(axis="y", linestyle="--", alpha=0.7)
    plt.show()

def plot_avg_sets():
    """Plots the average number of SETs vs. the number of cards.
    """
    num_cards = sorted(int(k) for k in results.keys())
    avg_sets = [results[str(n)]["avg_sets"] for n in num_cards]

    plt.figure(figsize=(8, 5))
    plt.plot(num_cards, avg_sets, marker="o", linestyle="-", color="b")
    plt.xlabel("Number of cards")
    plt.ylabel("Average number of SETs")
    plt.title("Average number of SETs vs. number of cards")
    plt.grid(True, linestyle="--", alpha=0.7)
    plt.show()

def plot_prob_set():
    """Plots the probability of at least one SET vs. the number of cards.
    """
    num_cards = sorted(int(k) for k in results.keys())
    prob_set = [1- results[str(n)]["prob_no_sets"] for n in num_cards]

    plt.figure(figsize=(8, 5))
    plt.plot(num_cards, prob_set, marker="s", linestyle="-", color="r")
    plt.xlabel("Number of cards")
    plt.ylabel("Probability of at least one SET (%)")
    plt.title("Probability of at least one SET vs. number of nards")
    plt.grid(True, linestyle="--", alpha=0.7)
    plt.show()

# Plot histograms for different numbers of cards
plot_histogram(num_cards)

# Plot average number of sets vs. number of cards
plot_avg_sets()

# Plot probability of no sets vs. number of cards
plot_prob_set()
