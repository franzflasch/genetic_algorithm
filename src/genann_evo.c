#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <genetic.h>
#include <genann.h>
#include <time.h>
#include <unistd.h>

#define POPULATION_SIZE 30
#define RETAIN_VALUE 0.3
#define MUTATIONS_PER_EVOLUTION 10
#define LOSER_LEVEL 6

#define RAND_RANGE 30 /* will be -(RAND_RANCGE/2) and +(RAND_RANCGE/2) */
#define STUCK_COUNT 10000


/* Input and expected out data for the XOR function. */
static const double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
static const double output[4] = {0, 1, 1, 0};
static genann *global_ann;

/* The lower the better */
double fitness(struct population_s *this, uint32_t individual_id)
{
   int i = 0;
   double err = 0;

   /* copy the weights from the individual to the neural network */
   for (i = 0; i < global_ann->total_weights; ++i) {
       global_ann->weight[i] = this->individuals[individual_id].genes[i];
   }

   /* Lets see how we did. */
   err = 0;
   err += fabs(*genann_run(global_ann, input[0]) - output[0]);
   err += fabs(*genann_run(global_ann, input[1]) - output[1]);
   err += fabs(*genann_run(global_ann, input[2]) - output[2]);
   err += fabs(*genann_run(global_ann, input[3]) - output[3]);

   return err;
}

/* The neural networks will get some random initial weights on creation */
void create_individual(struct population_s *this, uint32_t individual_id)
{
   int i = 0;

   /* Just randomize values */
   for(i=0;i<this->individuals[individual_id].size;i++)
   {
      this->individuals[individual_id].genes[i] = (double)rand_in_range(0,RAND_RANGE)-(RAND_RANGE/2);//((double)rand())/RAND_MAX-0.5;
   }
}

/* Every mutation will be some random value between -25 and +25 */
double mutate(struct population_s *this, uint32_t individual_id, uint32_t gene_index)
{
   double random_gene_value=this->individuals[individual_id].genes[gene_index];

   if(rand_in_range(0,1))
   {
      /* Add some random weight between -25 and +25 */
      random_gene_value += (double)rand_in_range(0,RAND_RANGE)-(RAND_RANGE/2);
   }
   else
   {
      /* Add some random weight between -25 and +25 */
      random_gene_value = (double)rand_in_range(0,RAND_RANGE)-(RAND_RANGE/2);  
   }
   return random_gene_value;
}

int main(int argc, char *argv[])
{
   int count = 0;
   int i = 0;
   srand(time(NULL));
   printf("GENANN example combined with genetic algorithm.\n");
   printf("Train a small ANN to the XOR function using genetic algorithm.\n");

   /* New network with 2 inputs,
   * 1 hidden layer of 2 neurons,
   * and 1 output. */
   global_ann = genann_init(2, 1, 2, 1);
   printf("Number of weights: %d\n", global_ann->total_weights);

   /* Create a population of  */
   population_t *pop = genetic_new(POPULATION_SIZE, global_ann->total_weights, RETAIN_VALUE, MUTATIONS_PER_EVOLUTION, LOSER_LEVEL, create_individual, fitness, mutate);

   printf("Evolve population, please wait...\n");
   while(pop->grade(pop) > 0.01)
   {
      count++;
      if(count%STUCK_COUNT == 0)
      {
         /* It seems were stuck, randomize some parents again */
             /* initialize individuals */
          printf("average fitness parents: %f\n", pop->grade(pop));
          for(i=0;i<(pop->retain_count);i++)
          {
            pop->create_individual_cb(pop, i);
          }
          printf("STUCK! %d\n", count);
          pop->print_population(pop);
          printf("average fitness parents: %f\n", pop->grade(pop));
      }

      //pop->print_population(pop);
      printf("average fitness parents: %f\n", pop->grade(pop));
      pop->evolve(pop);
      //usleep(10000);
   }
   //pop->print_population(pop);
   printf("Finished after %d evolutions.\n", count);
   printf("average fitness of the retained individuals after last evolution: %f\n", pop->grade(pop));
   /* Take the best one */
   printf("Weights: ");
   for (i = 0; i < global_ann->total_weights; ++i) {
       global_ann->weight[i] = pop->individuals[0].genes[i];
       printf("%.2f ", global_ann->weight[i]);
   }
   printf("\n");

   genetic_free(pop);

   /* Run the network and see what it predicts. */
   printf("Testing the output on the evolved neural network:\n");
   printf("Output for [%1.2f, %1.2f] is %1.2f.\n", input[0][0], input[0][1], *genann_run(global_ann, input[0]));
   printf("Output for [%1.2f, %1.2f] is %1.2f.\n", input[1][0], input[1][1], *genann_run(global_ann, input[1]));
   printf("Output for [%1.2f, %1.2f] is %1.2f.\n", input[2][0], input[2][1], *genann_run(global_ann, input[2]));
   printf("Output for [%1.2f, %1.2f] is %1.2f.\n", input[3][0], input[3][1], *genann_run(global_ann, input[3]));

   genann_free(global_ann);
   return 0;
}
