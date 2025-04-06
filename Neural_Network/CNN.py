import argparse
import logging
import os
import numpy as np
import matplotlib.pyplot as plt
import random
import time
from keras.layers import (Input, Dense, Dropout, Flatten, Conv2D, MaxPooling2D)
from keras.models import Model
from tensorflow.keras.callbacks import EarlyStopping, ReduceLROnPlateau
from tensorflow.keras.optimizers import Adam
from itertools import permutations

def process(file_path):
    """Reads Data.txt and returns the tables and number of sets formatted for the training the CNN
    """

    assert file_path.endswith('.txt')
    assert os.path.isfile(file_path)

    logging.info('Opening input file "%s"', file_path)
    with open(file_path, "r") as file:
        data = file.readlines()
    logging.info('Done opening file.')

    num_cards = int(data[1].split(":")[1].strip())
    num_attributes = int(data[2].split(":")[1].strip())
    num_tables = int(data[3].split(":")[1].strip())

    num_sets_start = data.index("# Number of SETs in each Table\n") + 2
    num_sets = np.array(list(map(int, data[num_sets_start].split())))

    tables_start = data.index("# Tables considered\n") + 2
    table_lines = [line.strip() for line in data[tables_start:] if line.strip()]

    tables = np.array(
        [[list(map(int, row.split())) for row in table_lines[i:i + num_cards]]
        for i in range(0, len(table_lines), num_cards)]
    )

    if tables.shape != (num_tables, num_cards, num_attributes):
        raise ValueError(f"Shape mismatch: expected {(num_tables, num_cards, num_attributes)}, got {tables.shape}")

    return tables, num_sets, num_cards, num_attributes, num_tables

def augment_data(tables, num_sets, num_cards, num_attributes, num_tables):
    """Augments the data by generating all permutations of tables with 1 SET and discarding a fraction of all
    tables with no SETs
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

    logging.info(f'Processed {len(tables)} tables of {num_cards} cards with {num_attributes} attributes')

    return filtered_tables, filtered_num_sets

def train_cnn(tables, num_sets, num_cards, num_attributes):
    """Trains a Convolutional Neural Network to predict SET formations.
    """

    inputs = Input(shape=(num_cards, num_attributes, 1))
    hidden = Conv2D(32, (1, 2), activation='relu', padding='same')(inputs)
    hidden = Conv2D(64, (1, 2), activation='relu', padding='same')(hidden)
    hidden = Conv2D(128, (1, 2), activation='relu', padding='same')(hidden)
    hidden = MaxPooling2D(pool_size=(1, 2), padding='same')(hidden)

    hidden = Flatten()(hidden)
    hidden = Dense(256, activation='relu')(hidden)
    hidden = Dropout(0.3)(hidden)
    hidden = Dense(128, activation='relu')(hidden)
    hidden = Dropout(0.3)(hidden)
    hidden = Dense(64, activation='relu')(hidden)


    outputs = Dense(1, activation='sigmoid')(hidden)  # Binary classification output

    model = Model(inputs=inputs, outputs=outputs)
    model.compile(loss='binary_crossentropy', optimizer=Adam(learning_rate=1e-3))
    model.summary()

    callbacks = [
        ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=5, min_lr=1e-6),
        EarlyStopping(monitor='val_loss', patience=15, restore_best_weights=True)
    ]

    history = model.fit(tables, num_sets, validation_split=0.2, epochs=150, batch_size=512, verbose=2, callbacks=callbacks)

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
    plt.savefig("CNN_loss.png")
    plt.show()

    return model


def test_cnn(model, tables, num_sets, num_random=10000):
    """Tests the trained model on random tables
    """
    print(f"\nTesting on {num_random} random tables:")
    indices = random.sample(range(len(tables)), num_random)
    tables_sample = tables[indices]
    num_sets_true = num_sets[indices]
    
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



_description = 'Convolutional Neural Network'

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=_description)
    parser.add_argument('infile', help='path to the input file')
    args = parser.parse_args()
    
    tables, num_sets, num_cards, num_attributes, num_tables = process(args.infile)
    tables, num_sets = augment_data(tables, num_sets, num_cards, num_attributes, num_tables)
    model = train_cnn(tables, num_sets, num_cards, num_attributes)
    test_cnn(model, tables, num_sets)

