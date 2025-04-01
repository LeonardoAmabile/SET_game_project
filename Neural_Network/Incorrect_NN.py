import numpy as np
from tensorflow import keras
from tensorflow.keras import layers
import matplotlib.pyplot as plt
import random
from keras.layers import (Input, Dense, Flatten)
from keras.models import Model
from tensorflow.keras.callbacks import EarlyStopping, ReduceLROnPlateau
from tensorflow.keras.optimizers import Adam

# DO NOT USE THIS VERSION OF THE NEURAL NETWORK, IT IS OUTDATED AND ONLY FOR ARCHIVE PURPOSES #


def parse_set_data(file_path):
    with open(file_path, 'r') as f:
        lines = f.readlines()

    # Get set counts
    counts_start = lines.index("# Number of SETs in each Table\n") + 2
    set_counts = list(map(int, lines[counts_start].strip().split()))

    # Get table lines
    tables_start = lines.index("# Tables considered\n") + 1
    raw_table_lines = [line.strip() for line in lines[tables_start:] if line.strip()]

    num_tables = len(set_counts)
    num_cards = 12
    num_attributes = 4
    tables = np.zeros((num_tables, num_cards, num_attributes), dtype=np.int32)

    for i in range(num_tables):
        table_lines = raw_table_lines[i * num_cards : (i + 1) * num_cards]
        table = [list(map(int, row.split())) for row in table_lines]
        tables[i] = np.array(table)

    return tables, np.array(set_counts)

# --- Load data ---
tables, set_counts = parse_set_data("../Dataset_generation/Data.txt")

X = tables.reshape(tables.shape[0], -1).astype("float32")
y = set_counts.astype("float32")

# --- Build the model ---
inputs = layers.Input(shape=(48,))
hidden = layers.Dense(128, activation="relu")(inputs)
hidden = layers.Dense(128, activation="relu")(hidden)
hidden = layers.Dense(64, activation="relu")(hidden)
outputs = layers.Dense(1)(hidden)

model = Model(inputs=inputs, outputs=outputs)
model.compile(loss='mse', optimizer=Adam(learning_rate=1e-3))
model.summary()

callbacks = [
    ReduceLROnPlateau(monitor='val_loss', factor=0.5, patience=10, min_lr=1e-6),
    EarlyStopping(monitor='val_loss', patience=20, restore_best_weights=True)
]


# --- Train ---
history = model.fit(X, y, validation_split=0.2, epochs=150, batch_size=512, verbose=2, callbacks=callbacks)

# --- Plot training loss ---
plt.figure(figsize=(8, 5))
plt.plot(history.history['loss'], label='Training Loss')
plt.plot(history.history['val_loss'], label='Validation Loss')
plt.xlabel("Epoch")
plt.ylabel("MSE Loss")
plt.title("Training vs Validation Loss")
plt.legend()
plt.grid(True, linestyle='--', alpha=0.6)
plt.tight_layout()
plt.savefig("bad_validation.png")
plt.show()

# --- Test on 10 random tables ---
print("\nTesting on 10 random tables:")
indices = random.sample(range(len(X)), 10)
X_sample = X[indices]
y_true = y[indices]
y_pred = model.predict(X_sample).flatten()

for i in range(10):
    true_val = y_true[i]
    pred_val = y_pred[i]
    print(f"Table {i+1}: True = {int(true_val)}, Predicted = {pred_val:.2f}")
