#We probably don't need half of these, let's clean it up later
import os
import numpy as np
import random
import matplotlib.pyplot as plt
import logging
import argparse
from keras.layers import Input, Dense, Dropout, Flatten, Conv2D, BatchNormalization, MaxPooling2D, Add, GlobalAveragePooling2D
from keras.models import Model
from tensorflow.keras.regularizers import l2
from tensorflow.keras.callbacks import EarlyStopping, ReduceLROnPlateau
from tensorflow.keras.losses import Huber
from tensorflow.keras.optimizers import Adam
from itertools import combinations, permutations
from keras.utils import to_categorical

logging.basicConfig(level=logging.INFO)


def process(file_path):
    """Processes input file to extract tables and number of SETs."""
    assert file_path.endswith('.txt') and os.path.isfile(file_path)
    
    print('Opening input file "%s"' % file_path)
    with open(file_path) as data:
        data = data.readlines()
    print('Done opening file.')
    
    num_cards = int(data[1].split(":")[1].strip())
    num_attributes = int(data[2].split(":")[1].strip())
    num_tables = int(data[3].split(":")[1].strip())
    
    num_sets_start = data.index("# Number of SETs in each Table\n") + 2
    num_sets = np.array(list(map(int, data[num_sets_start].strip().split())))
    
    if num_sets.shape[0] != num_tables:
        raise ValueError("Mismatch between number of tables and SET counts")
    
    tables_start = data.index("# Tables considered\n") + 2
    table_lines = [line.strip() for line in data[tables_start:] if line.strip()]
    
    tables = np.array(
        [[list(map(int, row.split())) for row in table_lines[i:i + num_cards]]
         for i in range(0, len(table_lines), num_cards)]
    )
    
    if tables.shape != (num_tables, num_cards, num_attributes):
        raise ValueError("Table dimensions do not match expected values")
    
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
    
    print('Processed %d tables of %d cards with %d attributes' % (len(filtered_tables), num_cards, num_attributes))
    
    return filtered_tables, filtered_num_sets, num_cards, num_attributes, len(filtered_tables)


def train_cnn(X, y, num_attributes):
    """Trains a CNN to predict whether a triplet of cards forms a SET."""
    inputs = Input(shape=(3, num_attributes, 1))
    
    x = Conv2D(32, (2, 2), activation='relu', padding='same')(inputs)
    x = BatchNormalization()(x)
    x = Conv2D(64, (2, 2), activation='relu', padding='same')(x)
    x = BatchNormalization()(x)
    x = MaxPooling2D(pool_size=(2, 2), padding='same')(x)
    
    x = Flatten()(x)
    x = Dense(128, activation='relu')(x)
    x = Dropout(0.3)(x)
    x = Dense(64, activation='relu')(x)
    x = Dropout(0.3)(x)
    
    outputs = Dense(1, activation='sigmoid')(x)  # Binary classification
    
    model = Model(inputs=inputs, outputs=outputs)
    model.compile(loss='binary_crossentropy', optimizer=Adam(learning_rate=1e-3))
    
    model.summary()
    
    # Train model
    callbacks = [
        ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=10, min_lr=1e-6),
        EarlyStopping(monitor='val_loss', patience=30, restore_best_weights=True)
    ]
    
    history = model.fit(X, y, validation_split=0.2, epochs=50, batch_size=512, verbose=2, callbacks=callbacks)
    
    # Plot loss and accuracy curves
    plt.plot(history.history["val_loss"], label="val_loss")
    plt.plot(history.history["loss"], label="loss")
    plt.legend()
    plt.show()


    # Test on a random table
    random_table = random.randint(0, num_tables - 1)
    test_table = np.expand_dims(tables[random_table], axis=0)
    predicted_sets = model.predict(test_table)
    real_sets = num_sets[random_table]
    print("Predicted SETs:", predicted_sets[0][0])
    print("Real SETs:", real_sets)


_description = 'Neural Network tests'

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=_description)
    parser.add_argument('infile', help='path to the input file')
    args = parser.parse_args()
    tables, num_sets, num_cards, num_attributes, num_tables = process(args.infile)
    model = train_cnn(tables, num_sets,num_attributes,)