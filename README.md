# genetic_algorithm
This is a simple genetic algorithm written in plain C.

It is inspired from https://lethain.com/genetic-algorithms-cool-name-damn-simple/
I wanted to do this example in C.

There are 2 examples:

- genetic_example: It is the same example also used in https://lethain.com/genetic-algorithms-cool-name-damn-simple/
- genetic_evo: In this example the genetic algorithm is combined with the neural network library from https://codeplea.com/genann
The XOR problem will be solved using the genetic algorithm, by only generating random weights and finding the best ones. You can see
that the solution will be found very quickly. Sometimes it lasts on a local optimum if the starting values are not optimal, however it should
find the peak after a few minutes in any case.

Have a nice Day!
