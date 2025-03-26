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

def process(file_path):
    """Main processing method."""
    assert file_path.endswith('.txt')
    assert os.path.isfile(file_path)

    logging.info('Opening input file "%s"', file_path)
    with open(file_path) as data:
        data = data.readlines()
    logging.info('Done opening file.')

    num_cards = int(data[1].split(":")[1].strip())
    num_attributes = int(data[2].split(":")[1].strip())
    num_tables = int(data[3].split(":")[1].strip())

    num_sets = np.zeros(num_tables, dtype=int)
    num_sets_start = data.index("# Number of SETs in each Table\n") + 2
    num_sets = np.array(list(map(int, data[num_sets_start].strip().split())))

    if num_sets.shape[0] != num_tables:
        raise ValueError("Number of SETs in each table does not match number of tables")

    tables_start = data.index("# Tables considered\n") + 2
    table_lines = data[tables_start:]

    filtered_lines = [line.strip() for line in table_lines if line.strip()]

    tables = np.array(
        [[list(map(int, row.split())) for row in filtered_lines[i : i + num_cards]]
        for i in range(0, len(filtered_lines), num_cards)]
    )
    
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

def train_cnn(X, y, num_cards, num_attributes):
    """Trains a Convolutional Neural Network to predict SET formations."""
    inputs = Input(shape=(num_cards, num_attributes, 1))
    
    x = Conv2D(256, (1, 2), activation='relu', padding='same')(inputs)
    x = BatchNormalization()(x)     
    x = Conv2D(128, (1, 2), activation='relu', padding='same')(x)
    x = BatchNormalization()(x)    
    x = Conv2D(64, (1, 2), activation='relu', padding='same')(x)
    x = BatchNormalization()(x)
    x = Conv2D(32, (1, 2), activation='relu', padding='same')(x)
    x = BatchNormalization()(x)
    x = Conv2D(16, (1, 2), activation='relu', padding='same')(x)
    x = BatchNormalization()(x)
    x = MaxPooling2D(pool_size=(1, 2), padding='same')(x)
    
    x = Flatten()(x)
    
    outputs = Dense(1, activation='sigmoid')(x)  # Binary classification output
    
    model = Model(inputs=inputs, outputs=outputs)
    model.compile(loss='binary_crossentropy', optimizer=Adam(learning_rate=1e-3))
    model.summary()
    
    callbacks = [
        ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=5, min_lr=1e-6),
        EarlyStopping(monitor='val_loss', patience=10, restore_best_weights=True)
    ]
    
    history = model.fit(X, y, validation_split=0.2, epochs=150, batch_size=512, verbose=2, callbacks=callbacks)
    
    plt.plot(history.history["val_loss"], label="Validation Loss")
    plt.plot(history.history["loss"], label="Training Loss")
    plt.legend()
    plt.show()
    
    return model


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Neural Network for SET detection')
    parser.add_argument('infile', help='Path to the input file')
    args = parser.parse_args()
    
    tables, num_sets, num_cards, num_attributes, num_tables = process(args.infile)
    
    augmented_tables, augmented_sets = augment_data(tables, num_sets)
    
    model = train_cnn(augmented_tables, augmented_sets, num_cards, num_attributes)

