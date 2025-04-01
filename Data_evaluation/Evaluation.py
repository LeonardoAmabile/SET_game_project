import subprocess
import matplotlib.pyplot as plt
import json
import time
import numpy as np
from math import comb
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

#     # Wait a bit to ensure the file is written
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


def plot_bar(num_cards, num_attributes, num_tables):
    """Plots a bar graph of the number of SETs for a given number of cards
    """
    if str(num_cards) not in results:
        print(f"No data found for {num_cards} cards.")
        return

    set_counts = results[str(num_cards)]["set_counts"]
    max_sets = max(set_counts)
    plt.figure(figsize=(8, 5))

    counts, bins= np.histogram(set_counts, bins=range(max_sets + 2), density=True) # I use this to get the x and y's for the bar graph (normalized)
    plt.bar(bins[:-1], counts, width=0.4, align='center', edgecolor='black', alpha=0.7, label="Simulated SETs")
    def gaussian(x, A, mu, sigma):
        return A * np.exp(-((x - mu) ** 2) / (2 * sigma ** 2))

    p0 = [max(counts), np.mean(set_counts), np.std(set_counts)]
    popt, pcov = curve_fit(gaussian, bins[:-1], counts, p0=p0)
    A_opt, mu_opt, sigma_opt = popt

    x_fit = np.linspace(bins[0], bins[-2], 200)
    plt.plot(x_fit, gaussian(x_fit, A_opt, mu_opt, sigma_opt), 'r-', label=f'Gaussian Fit (A={A_opt:.2f}, μ={mu_opt:.2f}, σ={sigma_opt:.2f})')

    plt.xlabel("Number of SETs")
    plt.ylabel("Probability")
    plt.legend()
    plt.title(f"Bar graph of SETs for {num_cards} cards, with {num_attributes} attributes, {num_tables} tables")
    plt.xticks(range(0, max_sets + 2, 3))
    plt.grid(axis="y", linestyle="--", alpha=0.7)
    plt.savefig(f"bar{num_cards}cards.png")
    plt.show()

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


def p(N):
    """Calculates the probability of at least one SET in N cards
    using recursion.
    """
    if not isinstance(N, int):
        raise ValueError("N must be an integer")
    elif N < 0:
        raise ValueError("N must be a non-negative integer")
    elif N < 3:
        return 0
    elif N == 3:
        return 1/79
    else:
        return (p(3)*comb(N-1,2)*(1-p(N-1))) + p(N-1)

def plot_prob_set(num_attributes, num_tables):
    """Plots the probability of at least one SET vs. the number of cards
    """
    num_cards = sorted(int(k) for k in results.keys())
    prob_set = 0.01 * np.array([100 - results[str(n)]["prob_no_sets"] for n in num_cards])
    
    # Errors on the simulated probabilities, assuming binomial distribution
    var_prob = prob_set * (1 - prob_set) / int(num_tables)
    std_prob = np.sqrt(var_prob)
    
    # Expected probabilities from recursive function p(n)
    expected_prob = np.zeros(len(num_cards))
    for n in num_cards:
        expected_prob[n - 1] = p(n)
    
    # Plots
    fig, (ax_main, ax_res) = plt.subplots(2, 1, sharex=True, figsize=(8, 6), gridspec_kw={'height_ratios': [4, 1]})
    ax_main.scatter(num_cards, expected_prob, color='b', marker='.', label='Expected')
    ax_main.errorbar(num_cards, prob_set, std_prob,color='r', marker='.', linestyle='none', label='Simulated')

    ax_main.set_ylabel("Probability of at least one SET")
    ax_main.set_title(f"Probability of at least one SET vs. number of cards,\n"f"with {num_attributes} attributes, {num_tables} tables")
    ax_main.grid(True, linestyle="--", alpha=0.7)
    ax_main.legend(loc='best')

    # Residuals
    residuals = prob_set - expected_prob
    ax_res.errorbar(num_cards, residuals, std_prob, color='black', marker='.', linestyle='none', label='Residuals')
    ax_res.axhline(0, color='gray', linestyle='--')
    ax_res.set_ylabel("Residuals")
    ax_res.set_xlabel("Number of cards")
    ax_res.grid(True, linestyle="--", alpha=0.7)

    chi2 = np.sum((residuals / std_prob) ** 2)
    dof = len(num_cards) - 1
    p_value = 1 - chi2.cdf(chi2, dof)
    print(f"Chi-squared: {chi2:.2f}, p-value: {p_value:.2f}")
    
    plt.subplots_adjust(hspace=0)
    plt.savefig("prob_set.png")
    plt.show()

# Plot the three graphs:
plot_bar(num_cards, num_attributes, num_tables)
plot_avg_sets(num_attributes, num_tables)
plot_prob_set(num_attributes, num_tables)