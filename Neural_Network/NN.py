import argparse
import logging
import os
import numpy as np
import matplotlib.pyplot as plt
from keras.layers import (Input, Dense, Dropout, Flatten, Conv2D,
                           BatchNormalization, MaxPooling2D)
from keras.models import Model
from tensorflow.keras.callbacks import EarlyStopping, ReduceLROnPlateau
from tensorflow.keras.optimizers import Adam
from itertools import permutations

def process(file_path):
    """Main processing method."""
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

    return filtered_tables, filtered_num_sets, num_cards, num_attributes, len(filtered_tables)

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
    
    return model



_description = 'Neural Network'

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=_description)
    parser.add_argument('infile', help='path to the input file')
    args = parser.parse_args()
    
    # Process input data
    tables, num_sets, num_cards, num_attributes, num_tables = process(args.infile)
    print(f"Shape of tables: {tables.shape}")

    # Augment data

    
    # Train the model
    model = train_nn(tables, num_sets, num_cards, num_attributes, num_tables)



