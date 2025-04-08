"""
Deep Neural Network (DNN) module for predicting if three cards form a SET.

This module processes boolean SET data (tables and SET presence) from a text file, 
reads and augments the data by permuting rows for tables that contain a SET, 
trains a deep neural network to predict if three cards form a SET, 
and tests the neural network on a seperate dataset
"""

import argparse
import logging
import os
import random
import time
import numpy as np
import matplotlib.pyplot as plt
from tensorflow.keras.layers import Input, Dense, Dropout, Flatten
from tensorflow.keras.models import Model
from tensorflow.keras.callbacks import EarlyStopping, ReduceLROnPlateau
from tensorflow.keras.optimizers import Adam
from itertools import permutations


def process(file_path):
    """
    Reads a txt file and returns the tables and number of SETs formatted for training.

    Parameters:
        file_path (str): The path to the input text file (ending with '.txt').

    Returns:
        tuple: A tuple containing:
            - tables (np.ndarray): An array of shape
              (num_tables, num_cards, num_attributes) representing the tables.
            - num_sets (np.ndarray): An array of integers with the SET counts per table.
            - num_cards (int): Number of cards per table.
            - num_attributes (int): Number of attributes per card.
            - num_tables (int): Total number of tables.
    """
    assert file_path.endswith('.txt')
    assert os.path.isfile(file_path)

    logging.info('Opening input file "%s"', file_path)
    with open(file_path, "r", encoding="utf-8") as file:
        data = file.readlines()
    logging.info('Done opening file.')

    num_cards = int(data[1].split(":")[1].strip())
    num_attributes = int(data[2].split(":")[1].strip())
    num_tables = int(data[3].split(":")[1].strip())

    num_sets_start = data.index("# Number of SETs in each Table\n") + 2
    num_sets = np.array(list(map(int, data[num_sets_start].split())))

    tables_start = data.index("# Tables considered\n") + 2
    table_lines = [line.strip() for line in data[tables_start:] if line.strip()]

    tables = np.array([
        [list(map(int, row.split()))
        for row in table_lines[i:i + num_cards]]
        for i in range(0, len(table_lines), num_cards)
    ])

    if tables.shape != (num_tables, num_cards, num_attributes):
        raise ValueError(
            f"Shape mismatch: expected {(num_tables, num_cards, num_attributes)}, got {tables.shape}"
        )

    return tables, num_sets, num_cards, num_attributes, num_tables

def augment_data(tables, num_sets, num_cards, num_attributes, num_tables):
    """
    Augments the data by generating all permutations of tables with 1 SET and discarding a fraction of
    all tables with no SETs.

    Parameters:
        tables (np.ndarray): Array of tables.
        num_sets (np.ndarray): Array of SET counts.
        num_cards (int): Number of cards per table.
        num_attributes (int): Number of attributes per card.
        num_tables (int): Total number of tables.

    Returns:
        tuple: (filtered_tables, filtered_num_sets) as np.ndarray objects.
    """
    filtered_tables = []
    filtered_num_sets = []
    zero_count = 0

    for i in range(num_tables):
        if num_sets[i] == 0:
            if zero_count % 10 == 0:
                filtered_tables.append(tables[i])
                filtered_num_sets.append(num_sets[i])
            zero_count += 1
        elif num_sets[i] == 1:
            card_permutations = list(np.array(list(permutations(tables[i]))))
            filtered_tables.extend(card_permutations)
            filtered_num_sets.extend([1] * len(card_permutations))
        else:
            filtered_tables.append(tables[i])
            filtered_num_sets.append(num_sets[i])


    filtered_tables = np.array(filtered_tables)
    filtered_num_sets = np.array(filtered_num_sets)

    logging.info(
        f'Processed {len(tables)} tables of {num_cards} cards with {num_attributes} attributes'
    )
    return filtered_tables, filtered_num_sets

def train_dnn(tables, num_sets, num_cards, num_attributes, num_tables):
    """
    Trains a deep neural network (DNN) to predict the number of SETs in a table.

    The network is built using a series of fully connected (dense) layers with dropout for
    regularization. The model is compiled with the Adam optimizer and binary crossentropy loss.

    Parameters:
        tables (np.ndarray): Array of tables with shape (num_samples, num_cards, num_attributes, 1).
        num_sets (np.ndarray): Array of SET counts (binary labels expected).
        num_cards (int): Number of cards per table.
        num_attributes (int): Number of attributes per card.
        num_tables (int): Total number of tables (used for logging).

    Returns:
        model (tensorflow.keras.models.Model): The trained model.
    """
    inputs = Input(shape=(num_cards, num_attributes, 1)) 
    hidden = Flatten()(inputs)  
    hidden = Dense(256, activation='relu')(hidden)
    hidden = Dropout(0.3)(hidden)
    hidden = Dense(128, activation='relu')(hidden)
    hidden = Dropout(0.3)(hidden)
    hidden = Dense(128, activation='relu')(hidden)
    hidden = Dense(64, activation='relu')(hidden)
    outputs = Dense(1, activation='sigmoid')(hidden)

    
    model = Model(inputs=inputs, outputs=outputs)
    model.compile(loss='binary_crossentropy', optimizer=Adam(learning_rate=1e-3))
    model.summary()
    
    callbacks = [
        ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=5, min_lr=1e-6),
        EarlyStopping(monitor='val_loss', patience=10, restore_best_weights=True)
    ]

    history = model.fit(
        tables, num_sets, validation_split=0.2, epochs=150,
        batch_size=512, verbose=2, callbacks=callbacks
    )

    plt.figure(figsize=(8, 5))
    plt.plot(history.history["val_loss"], label="Validation Loss")
    plt.plot(history.history["loss"], label="Training Loss")
    plt.xlabel("Epoch")
    plt.ylabel("BCE Loss")
    plt.xscale("log")
    plt.yscale("log")
    plt.title("Training vs Validation Loss")
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig("./Images/DNN_loss.png")
    plt.show()
    
    return model

def test_dnn(model, test_file_path, num_random=500):
    """
    Tests the trained model on random tables from a testing dataset.

    This function reads testing tables from the specified testing dataset file, applies
    augmentation, selects 'num_random' random tables, and uses the trained model to predict 
    the SET count (or binary label). It prints each table's true value and prediction if the
    difference exceeds 0.5, counts the total mismatches, and prints the total prediction time.

    Parameters:
        model (tensorflow.keras.models.Model): The trained Keras model.
        test_file_path (str): Path to the testing dataset file.
        num_random (int, optional): Number of random tables to test on (default: 500).

    Returns:
        None
    """
    test_tables, test_num_sets, test_num_cards, test_num_attributes, test_num_tables = process(test_file_path)
    test_tables, test_num_sets = augment_data(test_tables, test_num_sets, test_num_cards, test_num_attributes, test_num_tables)
    logging.info(f"\nTesting on {num_random} random tables:")
    indices = random.sample(range(len(test_tables)), num_random)
    tables_sample = test_tables[indices]
    num_sets_true = test_num_sets[indices]
    
    start_time = time.time()
    num_sets_pred = model.predict(tables_sample)
    elapsed_time = time.time() - start_time

    errors = 0
    for i in range(num_random):
        true_val = num_sets_true[i]
        pred_val = num_sets_pred[i][0]
        if np.abs(true_val - pred_val) > 0.5:
            print(f"Table {i+1}: True = {int(true_val)}, Predicted = {pred_val:.2f} (Mismatch)")
            errors += 1
    if errors == 0:
        print(f"{num_random} tables predicted correctly.")
    else:
        print(f"{errors} mismatches out of {num_random} tables.")
    print(f"\nTime taken to predict {num_random} tables: {elapsed_time:.4f} seconds")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Deep Neural Network for SET evaluation')
    parser.add_argument('infile', help='path to the input file')
    args = parser.parse_args()
    
    # Process and augment training data
    tables, num_sets, num_cards, num_attributes, num_tables = process(args.infile)
    tables, num_sets = augment_data(tables, num_sets, num_cards, num_attributes, num_tables)

    model = train_dnn(tables, num_sets, num_cards, num_attributes, num_tables)

    # Optionally, if the training file is the training dataset,
    # run tests on the testing dataset.
    if args.infile in ('./Training_dataset.txt', 'Training_dataset.txt'):
        test_dnn(model, 'Testing_dataset.txt', num_random=10000)
