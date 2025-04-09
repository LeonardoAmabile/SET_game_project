### SET_game_project
A programming project about the card game SET.

The SET game consists of identifying in a set of 12 cards a set of three winners. Each card has four attributes that distinguish it from the others:
 - Color (red, green, purple)
 - Shape (diamond, S, ellipse)
 - Filling (empty, shaded, full)
 - Number of figures (1, 2, 3)
A winning set (a SET) consists of three cards whose characteristics must meet certain requirements. For each of the 4 characteristics, the 3 cards
must have the same value (e.g., all 3 red) or different value (e.g., the three different shapes).

The purpose of the exercise is to find an optimized method to detect if a combination of three cards is a SET or not. 

First we will generate a table using the vector library in C++, we will create a match with a  number of cards n, and determine whether
a SET (a winning combination) is present analytically. Applying the algorithm on several randomly generated tables we will create a dataset with which to
train a NN that can take as input n cards and return an estimate of the probability that this table of n cards contains a SET.

Here’s the translated text in English:  

---

The algorithm can find all possible SETs within the given *n* cards in a more optimized way than simple brute-force. However, given the computational power available to us, we can only apply the neural network (NN) to 3 cards at a time and check whether they form a SET or not. Future implementations could involve applying the NN to a larger number of cards and detecting the presence of SETs.  

## **How to Use**  

First, you need to generate the dataset. After cloning the repository, run the following commands:  

- In linux:

```bash
cd SET_game_project
cd Dataset_generation
cmake -B build
cmake --build build
./my_program -o
```

- In Windows:

```bash
cd SET_game_project
cd Dataset_generation
rmdir /s /q build
mkdir build
cmake -G "MinGW Makefiles" -B build
cmake --build build
my_program -o
```

You will be asked for 3 inputs:  
- The number of cards  
- The number of attributes  
- The number of tables to generate  
- Boolean version (Y/n)  

For applying the dataset to the NNs in the *Neural_network* directory, it is recommended to use **3 cards**, a **small number of attributes (usually 4)**, a **large number of tables (more than 10,000)**, and set **Boolean version = Y**.  

The "Boolean version" refers to the type of output you want to obtain: either a series of 0s and 1s indicating whether at least one SET is present, or the raw number of SETs present in each table.  

A file named `Data.txt` will be generated or overwritten, containing all the dataset information and details about the presence of SETs.  

The tables are represented as vectors consisting exclusively of -1, 0, and 1. Each value corresponds to one of the possible attributes (e.g., *green, red, blue*; or *diamond, S, ellipse*).

After that, to use this data in a neural network, you need to switch to the **Neural_network** directory, where two different neural networks are available:  

- **DNN.py** is a deep neural network (DNN) with densely connected layers.  
- **CNN.py** is a convolutional neural network (CNN) that leverages the invariance of the result under row permutations to optimize the learning process.

```bash
cd SET_game_project
cd Neural_Network
python3 dnn.py ../Dataset_generation/Data.txt
```

Or if you prefer to use the CNN:

```bash
cd SET_game_project
cd Neural_Network
python3 cnn.py ../Dataset_generation/Data.txt
```

There is already a training dataset present in the Neural Networks folder, that is used as a robust example of the capabilities of the NN. If this is used, 10000 tables from a separate dataset will be evaluated by the model to test efficiency. To access simply:

```bash
cd SET_game_project
cd Neural_Network
python3 dnn.py ./Training_dataset.txt
```

If you want to evaluate the model on a pre-created dataset (different from the training dataset) to check the number of errors youcan type:

```bash
cd SET_game_project
cd Neural_Network
python3 dnn.py ./Testing_dataset.txt
```

## Requirements
# Dataset_generation in C++:
- compiler g++ 11.4.0
- cmake version 3.22.1
- gcc (Rev3, Built by MSYS2 project) 13.2.0 (On Windows)
# Neural_network in Python
-Python 3.8.10
-Numpy 1.24.3
-Matplotlib 3.5.1
-Keras 2.13.1
