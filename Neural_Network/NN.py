import os
import numpy as np
import random
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from keras.layers import Input,Dense,Dropout,Flatten
from keras.models import Model
from tensorflow.keras.callbacks import EarlyStopping,ReduceLROnPlateau
from tensorflow.keras.optimizers import Adam

import argparse
from math import *
import logging
logging.basicConfig(level=logging.INFO)

def process(file_path):
    """Main processing method.
    """
    # Make sure that the file_argument points to an
    # existing text file.
    assert file_path.endswith('.txt')
    assert os.path.isfile(file_path)

    # Open the input file (note that we are taking advantage of context
    # management, using a with statement).
    logging.info('Opening input file "%s"', file_path)
    with open(file_path) as data:
        data = data.readlines()
    logging.info('Done opening file.')

    # Extract parameters, looking specifically at lines 2,3,4
    num_cards = int(data[1].split(":")[1].strip())
    num_attributes = int(data[2].split(":")[1].strip())
    num_tables = int(data[3].split(":")[1].strip())

    # Extract number of SETs in each table, starting at the correct line and looping from there
    num_sets = np.zeros(num_tables, dtype=int)
    num_sets_start = data.index("# Number of SETs in each Table\n") + 2
    num_sets = np.array(list(map(int, data[num_sets_start].strip().split())))

    # Check that the number of SETs in each table matches the number of tables
    if num_sets.shape[0] != num_tables:
        raise ValueError("Number of SETs in each table does not match number of tables")


    # Extract the relevant lines that include our tables
    tables_start = data.index("# Tables considered\n") + 2
    table_lines = data[tables_start:]

    # Filter out empty lines (table separators)
    filtered_lines = [line.strip() for line in table_lines if line.strip()]

    # Process tables into a numpy array, creating a new table every num_cards lines
    tables = np.array(
        [[list(map(int, row.split())) for row in filtered_lines[i : i + num_cards]]
        for i in range(0, len(filtered_lines), num_cards)]
    )
    
    # Some tests to make sure the data is correct
    if tables.shape[0] != num_tables:
        raise ValueError("Number of tables does not match number of tables")
    if tables.shape[1] != num_cards:
        raise ValueError("Number of cards does not match number of cards")
    if tables.shape[2] != num_attributes:
        raise ValueError("Number of attributes does not match number of attributes")
    

    logging.info('Found %d tables of %d cards with %d attributes', num_tables, num_cards, num_attributes)
    logging.info('Shape of the tables: %s', tables.shape)
    return tables,num_sets,num_cards,num_attributes, num_tables

def augment_data(tables, num_sets, max_permutations=5):
    """Generates augmented data by permuting the tables."""
    augmented_tables, augmented_num_sets = [], []
    
    for i in range(len(tables)):
        table = tables[i]
        sets_count = num_sets[i]
        
        # Generate a limited number of permutations to avoid redundancy
        generated = 0
        stored_permutations = []
        while generated < max_permutations:
            permuted_table = np.random.permutation(table)
            if not any(np.array_equal(permuted_table, p) for p in stored_permutations):
                stored_permutations.append(permuted_table)
                augmented_tables.append(permuted_table)
                augmented_num_sets.append(sets_count)
                generated += 1
    
    return np.array(augmented_tables), np.array(augmented_num_sets)

def train_nn(tables, num_sets, num_cards, num_attributes, num_tables):
    """Train a neural network to predict the number of SETs in a table."""
    
    inputs = Input(shape=(num_cards, num_attributes))   
    hidden = Dense(512, activation='relu')(inputs)
    hidden = Dropout(0.3)(hidden)
    hidden = Dense(256, activation='relu')(hidden)
    hidden = Dropout(0.3)(hidden)
    hidden = Dense(128, activation='relu')(hidden)
    hidden = Dropout(0.3)(hidden)
    hidden = Dense(56, activation='relu')(hidden)
    outputs = Dense(1)(hidden)

    
    model = Model(inputs=inputs, outputs=outputs)
    model.compile(loss='binary_crossentropy', optimizer=Adam(learning_rate=1e-3))
    model.summary()
    
    # Define callbacks
    callbacks = [
        ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=10, min_lr=1e-6),
        EarlyStopping(monitor='val_loss', patience=15, restore_best_weights=True)
    ]
    
    # Train the model
    history = model.fit(tables, num_sets, validation_split=0.2, epochs=150, batch_size=512, verbose=2, callbacks=callbacks)
    
    # Check shapes of tables and num_sets
    print(f"Shape of tables: {tables.shape}")
    print(f"Shape of num_sets: {num_sets.shape}")
    
    
    # Modifica questa parte
    plt.plot(history.history["val_loss"])  # 'history' invece di 'model.history'
    plt.plot(history.history["loss"])     # 'history' invece di 'model.history'
    plt.legend(["val_loss", "loss"])
    plt.show()

    
    random_table = random.randint(0, num_tables - 1)
    test_table = tables[random_table]  # Take a random table as a test
    test_table = np.expand_dims(test_table, axis=0)  # Shape becomes (1, num_cards, num_attributes)
    
    # Get prediction
    predicted_sets = model.predict(test_table)
    real_sets = num_sets[random_table]
    
    logging.info("Estimated number of sets: %f", predicted_sets[0][0])
    logging.info("Real number of sets: %f", real_sets)



_description = 'Neural Network tests'

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=_description)
    parser.add_argument('infile', help='path to the input file')
    args = parser.parse_args()
    
    # Process input data
    tables, num_sets, num_cards, num_attributes, num_tables = process(args.infile)
    print(f"Shape of tables: {tables.shape}")

    # Augment data
    augmented_tables, augmented_sets = augment_data(tables, num_sets)
    print(f"Shape of augmented tables: {augmented_tables.shape}")
    
    # Train the model
    model = train_nn(augmented_tables, augmented_sets, num_cards, num_attributes, num_tables)



