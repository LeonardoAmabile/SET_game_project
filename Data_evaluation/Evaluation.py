"""
Evaluation module for analyzing SET distributions and simulation results.
"""
import subprocess
import json
import time
from math import comb
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

def run_cpp_program(num_cards, num_attributes, num_tables, cpp_executable, output_file="data.txt"):
    """
    Runs the specified C++ executable with given inputs and returns the processed data.

    This function builds a command-line input string with the specified number of cards, 
    attributes, and tables, followed by a confirmation line. It invokes the C++ program
    using the provided executable path, adding the '-o' flag for optimized mode.

    Parameters:
        num_cards (int): The number of cards to include per table.
        num_attributes (int): The number of attributes each card has.
        num_tables (int): The total number of tables to generate.
        cpp_executable (str): The file path to the compiled C++ executable.
        output_file (str, optional): The file name or path where the C++ program writes its output.
                                     Defaults to "data.txt".

    Returns:
        tuple: The parsed data from the output file, as returned by the `parse_data_file` function.

    Raises:
        RuntimeError: If the C++ executable returns a non-zero exit code, indicating 
                      an error during execution.
        AssertionError: If the provided `cpp_executable` path is invalid or the output 
                        file is not found, depending on the implementation of `parse_data_file`.
    """
    input_str = f"{num_cards}\n{num_attributes}\n{num_tables}\nn\n"
    command = [cpp_executable]
    command.append("-o")

    result = subprocess.run(
        command,
        input=input_str,
        text=True,
        capture_output=True,
        check=False
    )
    if result.returncode != 0:
        raise RuntimeError(f"Error running C++ program: {result.stderr}")

    # Wait a bit to ensure the file is written
    time.sleep(0.5)

    return parse_data_file(output_file)

def parse_data_file(file_path):
    """
    Parse the output file generated by the C++ program and extract relevant data.

    This function reads a text file containing results from multiple SET tables. It looks for
    specific markers in the file to identify:
    - The number of SETs found in each generated table
    - The overall probability of having no SETs
    - The average number of SETs across all tables

    Args:
        file_path (str): The path to the output file generated by the C++ program.

    Returns:
        tuple:
            - set_counts (list of int): List of SET counts for each table.
            - prob_no_sets (float): Probability of generating 
                                    a table with no SETs (as a percentage).
            - avg_sets (float): Average number of SETs across all tables.

    Raises:
        ValueError: If expected markers are missing or the file format is unexpected.
    """
    with open(file_path, "r", encoding="utf-8") as file:
        lines = file.readlines()

    set_counts = []
    prob_no_sets = None
    avg_sets = None

    set_counts_start = lines.index("# Number of SETs in each Table\n") + 2
    set_counts = list(map(int, lines[set_counts_start].strip().split()))

    for line in lines:
        if line.startswith("# Probability of no SETs:"):
            prob_no_sets = float(line.split(":")[1].strip().replace("%", ""))
        elif line.startswith("# Average number of SETs:"):
            avg_sets = float(line.split(":")[1].strip())

    return set_counts, prob_no_sets, avg_sets

def plot_bar(num_cards, num_attributes, num_tables):
    """
    Plot a normalized bar graph showing the distribution of SET counts and fit a Gaussian curve.

    This function visualizes the distribution of the number of SETs found in randomly generated
    SET tables with the specified number of cards and attributes. It retrieves the relevant
    data from the global `results` dictionary, plots a normalized histogram (as a bar chart),
    and fits a Gaussian curve to the distribution.

    The resulting plot is saved as a PNG file named according to the number of cards.

    Args:
        num_cards (int): Number of cards per table used in the simulation.
        num_attributes (int): Number of attributes per card (for labeling purposes).
        num_tables (int): Number of tables generated in the simulation (for labeling purposes).

    Returns:
        None

    Raises:
        KeyError: If `num_cards` is not found in the `results` dictionary.

    Side Effects:
        - Displays a matplotlib plot
        - Saves the plot as an image file (e.g., "bar12cards.png")
    """
    if str(num_cards) not in results:
        print(f"No data found for {num_cards} cards.")
        return

    set_counts = results[str(num_cards)]["set_counts"]
    max_sets = max(set_counts)
    plt.figure(figsize=(8, 5))

    counts, bins= np.histogram(
                set_counts, bins=range(max_sets + 2),
                density=True
            )

    plt.bar(
        bins[:-1], counts, width=0.4,
        align='center', edgecolor='black', alpha=0.7,
        label="Simulated SETs"
    )

    def gaussian(x, a, mu, sigma):
        return a * np.exp(-((x - mu) ** 2) / (2 * sigma ** 2))

    p0 = [max(counts), np.mean(set_counts), np.std(set_counts)]
    popt, _ = curve_fit(gaussian, bins[:-1], counts, p0=p0) # pylint: disable=unbalanced-tuple-unpacking
    amp_opt, mu_opt, sigma_opt = popt

    x_fit = np.linspace(bins[0], bins[-2], 200)
    plt.plot(
        x_fit, gaussian(x_fit, amp_opt, mu_opt, sigma_opt), 'r-',
        label=f'Gaussian Fit (A={amp_opt:.2f}, μ={mu_opt:.2f}, σ={sigma_opt:.2f})'
    )

    plt.xlabel("Number of SETs")
    plt.ylabel("Probability")
    plt.legend()
    plt.title(
        f"Bar graph of SETs for {num_cards} cards, "
        f"with {num_attributes} attributes, "
        f"{num_tables} tables"
    )

    plt.xticks(range(0, max_sets + 2, 3))
    plt.grid(axis="y", linestyle="--", alpha=0.7)
    plt.savefig(f"./Images/bar{num_cards}cards.png")
    plt.show()

def plot_avg_sets(num_attributes, num_tables):
    """
    Plot the average number of SETs as a function of the number of cards.

    This function retrieves the average number of SETs for each card count from the global
    `results` dictionary and generates a scatter plot. The x-axis represents the number
    of cards, and the y-axis shows the corresponding average number of SETs computed over
    a given number of tables.

    The resulting plot is saved as a PNG file named "avg_sets.png".

    Args:
        num_attributes (int): Number of attributes per card (used for labeling).
        num_tables (int): Number of tables generated for each card count (used for labeling).

    Returns:
        None

    Side Effects:
        - Displays a matplotlib plot
        - Saves the plot as "avg_sets.png"
    """
    num_cards = sorted(int(k) for k in results.keys())
    avg_sets = [results[str(n)]["avg_sets"] for n in num_cards]

    plt.figure(figsize=(8, 5))
    plt.scatter(num_cards, avg_sets, marker="o", linestyle="-", color="b")
    plt.xlabel("Number of cards")
    plt.ylabel("Average number of SETs")
    plt.title(
        f"Average number of SETs vs. number of cards, "
        f"with {num_attributes} attributes, "
        f"{num_tables} tables"
    )

    plt.grid(True, linestyle="--", alpha=0.7)
    plt.savefig("./Images/avg_sets.png")
    plt.show()

def p(n):
    """
    Recursively calculate the probability of finding at least one SET among n cards.

    This function estimates the probability of at least one SET existing in a group of n cards,
    using a recursive model based on the known probability for n=3 and incremental probabilities
    for larger values. 
    The function uses combinatorial logic to determine the likelihood of finding a SET.

    Args:
        n (int): The number of cards.

    Returns:
        float: The estimated probability of finding at least one SET among n cards.

    Raises:
        ValueError: If n is not an integer or is negative.
    """

    if not isinstance(n, int):
        raise ValueError("n must be an integer")
    if n < 0:
        raise ValueError("n must be a non-negative integer")
    if n < 3:
        return 0
    if n == 3:
        return 1/79
    return (p(3)*comb(n-1,2)*(1-p(n-1))) + p(n-1)

def plot_prob_set(num_attributes, num_tables):
    """
    Plot the probability of finding at least one SET as a function of the number of cards.

    This function compares the simulated probability of finding at least one SET in a group
    of cards (based on data in the `results` dictionary) with the theoretical values 
    computed using the recursive function `p(n)`. It also includes error bars based on 
    binomial variance and shows the residuals between simulated and expected values.

    The result is displayed in two stacked subplots:
        - Top plot: Simulated vs. expected probability of at least one SET
        - Bottom plot: Residuals (difference between simulated and expected)

    The plot is saved as "prob_set.png".

    Args:
        num_attributes (int): Number of attributes per card (used for labeling).
        num_tables (int): Number of tables used per card count (used to estimate uncertainty).

    Returns:
        None

    Side Effects:
        - Displays a matplotlib plot
        - Saves the plot as "prob_set.png"
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
    _, (ax_main, ax_res) = plt.subplots(
        2, 1, sharex=True, figsize=(8, 6),
        gridspec_kw={'height_ratios': [4, 1]}
    )
    ax_main.scatter(num_cards, expected_prob, color='b', marker='.', label='Expected')
    ax_main.errorbar(
        num_cards, prob_set, std_prob,
        color='r', marker='.', linestyle='none', label='Simulated'
    )

    ax_main.set_ylabel("Probability of at least one SET")
    ax_main.set_title(
        f"Probability of at least one SET vs. number of cards,\n"
        f"with {num_attributes} attributes, {num_tables} tables"
    )
    ax_main.grid(True, linestyle="--", alpha=0.7)
    ax_main.legend(loc='best')

    # Residuals
    residuals = prob_set - expected_prob
    ax_res.errorbar(
        num_cards, residuals, std_prob,
        color='black', marker='.', linestyle='none', label='Residuals'
    )
    ax_res.axhline(0, color='gray', linestyle='--')
    ax_res.set_ylabel("Residuals")
    ax_res.set_xlabel("Number of cards")
    ax_res.grid(True, linestyle="--", alpha=0.7)

    plt.subplots_adjust(hspace=0)
    plt.savefig("./Images/prob_set.png")
    plt.show()

# Configuration
num_cards_ = input("Enter the number of cards: ")
num_attributes_ = input("Enter the number of attributes: ")
num_tables_ = input("Enter the number of tables: ")
cpp_executable_ = input("Enter the path to your compiled C++ program: ")

# Dictionary to store results
results = {}

for n_cards in range(1, int(num_cards_)+1):
    print(f"Running for {n_cards} cards...")
    set_counts_, prob_no_sets_, avg_sets_ = run_cpp_program(
        n_cards, num_attributes_, num_tables_, cpp_executable_
    )
    results[n_cards] = {
        "set_counts": set_counts_,
        "prob_no_sets": prob_no_sets_,
        "avg_sets": avg_sets_
    }

# Save results to a JSON file
with open("set_results.json", "w", encoding="utf-8") as f:
    json.dump(results, f, indent=4)

print("All simulations completed!")

with open("set_results.json", "r", encoding="utf-8") as f:
    results = json.load(f)


plot_bar(num_cards_, num_attributes_, num_tables_)
plot_avg_sets(num_attributes_, num_tables_)
plot_prob_set(num_attributes_, num_tables_)
