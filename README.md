# SET_game_project
A programming project about the card game SET.

The SET game consists of identifying in a set of 12 cards a set of three winners. Each card has four attributes that distinguish it from the others:
 - Color (red, green, purple)
 - Shape (diamond, S, ellipse)
 - Filling (empty, shaded, full)
 - Number of figures (1, 2, 3)
A winning set (a SET) consists of three cards whose characteristics must meet certain requirements. For each of the 4 characteristics, the 3 cards
must have the same value (e.g., all 3 red) or different value (e.g., the three different shapes).

The purpose of the exercise is to estimate the minimum number of cards that guarantees the tates the existence of at least one SET as a function of the number of attributes.
In the literature such a value is known up to n_attributes = 5, and we are interested inconfirm the results already obtained and estimate a solution for n_att greater

First we will generate a table using the vector library in C++, we will create a match with a small number of cards n, and determine whether
a SET (a winning combination) is present analytically. Applying the algorithm on several randomly generated tables we will create a dataset with which to
train a DNN that can take as input n cards and return an estimate of the probability that this table of n cards contains a SET.
 
After that, N > n cards will be generated, and n will be chosen (randomly) for the DNN to analyze, giving in output a probability of the presence of a SET in those n cards.
WWe'll apply this algorythm multiple times in way to take all the possible informations on the N cards.

The application of the algorithm for multiple matches of N cards, will go on to estimate the probability of finding a SET in a table of N cards.

Applying the same procedure for different values of N we will be able to create a graph with the probability of having at least one SET in a hand, thus giving an
estimate of the minimum value of cards needed to have at least one combination winner.

This whole process will then be repeated by changing the number of at tributes in order to compare our results with those found in the literature:

#attributes    #min_cards
1               2
2               4
3               9
4               20
5               45
6               112-114
7               unknown
    
