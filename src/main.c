#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <genetic.h>
#include <unistd.h>

#define INDIVIDUAL_SIZE 6
#define POPULATION_SIZE 15

#define TARGET_VALUE 12
#define MIN_VALUE 0
#define MAX_VALUE 100

#define RETAIN_VALUE 0.3
#define MUTATIONS_PER_EVOLUTION 4
#define LOSER_LEVEL 4


double fitness(struct population_s *this, uint32_t individual_id)
{
   int sum = 0;
   int i = 0;

   for(i=0;i<this->individuals[individual_id].size;i++)
   {
      sum += this->individuals[individual_id].genes[i];
   }

   return abs(sum-TARGET_VALUE);
}

void create_individual(struct population_s *this, uint32_t individual_id, uint32_t gene_index)
{
   int i = 0;

   /* Just randomize values */
   for(i=0;i<this->individuals[individual_id].size;i++)
   {
      this->individuals[individual_id].genes[i] = rand_in_range(0, 100);
   }
}

double mutate(struct population_s *this, uint32_t individual_id)
{   
   uint32_t random_gene_value=0;

   /* Add some random individual */
   random_gene_value = rand_in_range(MIN_VALUE, MAX_VALUE);

   return random_gene_value;
}

int main() 
{
   int i = 0;
   srand(time(NULL));

   population_t *pop = genetic_new(POPULATION_SIZE, INDIVIDUAL_SIZE, RETAIN_VALUE, MUTATIONS_PER_EVOLUTION, LOSER_LEVEL, create_individual, fitness, mutate);

   for(i=0;i<100;i++)
   {
      pop->print_population(pop);
      printf("average fitness parents: %f\n", pop->grade(pop));
      pop->evolve(pop);
      usleep(200000);
   }

   // pop->target = 150;
   // for(i=0;i<10;i++)
   // {
   //    pop->print_population(pop);
   //    printf("average fitness %f\n", pop->grade(pop));
   //    pop->evolve(pop);
   //    usleep(10000);
   // }


   genetic_free(pop);

   // for(i=0;i<100;i++)
   // {
   //    printf("%f\n", ((double)rand())/RAND_MAX-0.5);
   // }

   return 0;
}
