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

logging.basicConfig(level=logging.INFO)

def process(file_path):
    """Processes input file to extract tables and number of SETs."""
    assert file_path.endswith('.txt') and os.path.isfile(file_path)
    
    logging.info('Opening input file "%s"', file_path)
    with open(file_path) as data:
        data = data.readlines()
    logging.info('Done opening file.')
    
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
    
    logging.info('Found %d tables of %d cards with %d attributes', num_tables, num_cards, num_attributes)
    logging.info('Shape of the tables: %s', tables.shape)
    
    return tables, num_sets, num_cards, num_attributes, num_tables

def train_nn(tables, num_sets, num_cards, num_attributes, num_tables):
    """Train a CNN-based neural network to estimate the number of SETs."""
    
    # Add channel dimension for CNN
    tables = np.expand_dims(tables, axis=-1)

    inputs = Input(shape=(num_cards, num_attributes, 1))
    
    # Convolutional Block 1
    x = Conv2D(32, (3, 3), activation='relu', padding='same', kernel_regularizer=l2(1e-4))(inputs)
    x = BatchNormalization()(x)
    x = Conv2D(32, (3, 3), activation='relu', padding='same', kernel_regularizer=l2(1e-4))(x)
    x = BatchNormalization()(x)
    x = MaxPooling2D(pool_size=(2, 2))(x)
    
    # Convolutional Block 2
    x = Conv2D(64, (3, 3), activation='relu', padding='same', kernel_regularizer=l2(1e-4))(x)
    x = BatchNormalization()(x)
    x = Conv2D(64, (3, 3), activation='relu', padding='same', kernel_regularizer=l2(1e-4))(x)
    x = BatchNormalization()(x)
    x = MaxPooling2D(pool_size=(2, 2))(x)
    
    # Convolutional Block 3 (Residual Connection)
    shortcut = Conv2D(128, (1, 1), padding='same', kernel_regularizer=l2(1e-4))(x)
    x = Conv2D(128, (3, 3), activation='relu', padding='same', kernel_regularizer=l2(1e-4))(x)
    x = BatchNormalization()(x)
    x = Conv2D(128, (3, 3), activation='relu', padding='same', kernel_regularizer=l2(1e-4))(x)
    x = BatchNormalization()(x)
    x = Add()([x, shortcut])  # Residual connection
    x = GlobalAveragePooling2D()(x)  # Avoids dimension issues    
    x = Flatten()(x)
    
    # Fully Connected Layers
    x = Dense(256, activation='relu', kernel_regularizer=l2(1e-4))(x)
    x = Dropout(0.5)(x)
    x = Dense(128, activation='relu', kernel_regularizer=l2(1e-4))(x)
    x = Dropout(0.3)(x)
    
    outputs = Dense(1)(x)  # Regression output

    model = Model(inputs=inputs, outputs=outputs)
    model.compile(loss=Huber(delta=1.0), optimizer=Adam(learning_rate=1e-3))

    model.summary()

    # Learning rate scheduler and early stopping
    callbacks = [
        ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=5, min_lr=1e-6),
        EarlyStopping(monitor='val_loss', patience=10, restore_best_weights=True)
    ]

    history = model.fit(tables, num_sets, validation_split=0.2, epochs=100, batch_size=512, verbose=2, callbacks=callbacks)

    # Plot loss curves
    plt.plot(history.history["val_loss"])
    plt.plot(history.history["loss"])
    plt.legend(["val_loss", "loss"])
    plt.show()

    # Test on a random table
    random_table = random.randint(0, num_tables - 1)
    test_table = np.expand_dims(tables[random_table], axis=0)
    predicted_sets = model.predict(test_table)
    real_sets = num_sets[random_table]
    logging.info("Estimated number of sets: %f", predicted_sets[0][0])
    logging.info("Real number of sets: %f", real_sets)

_description = "CNN"

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=_description)
    parser.add_argument('infile', help='path to the input file')
    args = parser.parse_args()
    tables, num_sets, num_cards, num_attributes, num_tables = process(args.infile)
    train_nn(tables, num_sets, num_cards, num_attributes, num_tables)


