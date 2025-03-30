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


def train_cnn(X, y, num_cards, num_attributes):
  """Trains a Convolutional Neural Network to predict SET formations."""
  inputs = Input(shape=(num_cards, num_attributes, 1))

  x = Conv2D(32, (1, 2), activation='relu', padding='same')(inputs)
  x = Conv2D(64, (1, 2), activation='relu', padding='same')(x)
  x = Conv2D(128, (1, 2), activation='relu', padding='same')(x)
  x = MaxPooling2D(pool_size=(1, 2), padding='same')(x)

  x = Flatten()(x)

  # Nuovi strati Fully Connected
  x = Dense(256, activation='relu')(x)
  x = Dropout(0.3)(x)
  x = Dense(128, activation='relu')(x)
  x = Dropout(0.3)(x)
  x = Dense(64, activation='relu')(x)

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
    
    model = train_cnn(tables, num_sets, num_cards, num_attributes)

