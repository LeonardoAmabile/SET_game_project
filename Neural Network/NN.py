import os
os.environ["TF_ENABLE_ONEDNN_OPTS"] = "0"
import numpy as np
import random
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from keras.layers import Input,Dense,Dropout,Flatten,LeakyReLU
from keras.models import Model
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
    for i in range(num_tables):
        num_sets[i] = int(data[num_sets_start][2*i])  # Convert to integer and store

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

def train_nn(tables,num_sets,num_cards,num_attributes, num_tables):
    """Train a neural network to predict the number of SETs in a table.
    """
    # Now we can start processing the data
    inputs=Input(shape=(num_cards,num_attributes))
    hidden=Dense(25, activation='relu')(inputs)
    hidden=Dense(25, activation='relu')(hidden)
    hidden=Dense(25, activation='relu')(hidden)
    hidden=Dense(25, activation='relu')(hidden)
    hidden=Dense(25, activation='relu')(hidden)

    hidden=Dense(25, activation='relu')(hidden)
    outputs = Dense(1)(hidden)
    deepmodel = Model(inputs=inputs, outputs=outputs)
    deepmodel.compile(loss='MSE', optimizer='adam')
    deepmodel.summary()
    deephistory=deepmodel.fit(tables,num_sets,validation_split=0.5,epochs=150,verbose=0)
    print(deephistory.history.keys())
    plt.plot(deephistory.history["val_loss"])
    plt.plot(deephistory.history["loss"])
    plt.legend(["val_loss","loss"])
    plt.show()

    random_table = random.randint(0, num_tables)
    test_table = tables[random_table] #Take a random table as a test
    test_table = np.expand_dims(test_table, axis=0)   # Shape becomes (1, num_cards, num_attributes)

    # Get prediction
    predicted_sets = deepmodel.predict(test_table)
    real_sets = num_sets[random_table]

    # Since the output is an array, extract the single value
    logging.info("Estimated number of sets: %f", predicted_sets[0][0])
    logging.info("Real number of sets: %f", real_sets)

_description = 'Neural Network tests'






if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=_description)
    parser.add_argument('infile', help='path to the input file')
    args = parser.parse_args()
    tables, num_sets, num_cards, num_attributes, num_tables = process(args.infile)
    train_nn(tables, num_sets, num_cards, num_attributes, num_tables)

