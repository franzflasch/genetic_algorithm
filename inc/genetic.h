#ifndef __GENETIC_H__
#define __GENETIC_H__

#include <stdlib.h>
#include <stdint.h>

/* With this option enabled, genes can be placed on different 
 * places than the parents
 */
#define USE_GENE_MIXING 0

typedef struct individual_s
{
    double fitness_val;
    uint32_t size;
    double *genes;

} individual_t;

typedef struct population_s
{
    uint32_t count;
    individual_t *individuals;

    uint32_t retain_count;

    uint32_t mutations;
    uint32_t loser_level;

    /* public methods */
    void (*print_individual)(struct population_s *this, uint32_t individual_id);
    void (*print_population)(struct population_s *this);
    double (*get_fitness)(struct population_s *this, uint32_t individual_id);
    double (*grade)(struct population_s *this);
    void (*evolve)(struct population_s *this);

    /* user callbacks */
    double (*fitness_cb)(struct population_s *this, uint32_t individual_id);
    void (*create_individual_cb)(struct population_s *this, uint32_t individual_id);
    double (*mutate_cb)(struct population_s *this, uint32_t individual_id, uint32_t gene_index);

} population_t;

population_t *genetic_new(uint32_t count, 
                          uint32_t genom_size, 
                          double retain_val, 
                          uint32_t mutation_level, 
                          uint32_t loser_level, 
                          void *create_individual_cb, 
                          void *fitness_cb, 
                          void *mutate_cb);
void genetic_free(population_t *this);

/* Common helper functions */
static uint32_t rand_in_range(int min, int max)
{
   double scaled = (double)rand()/RAND_MAX;
   return (max - min +1)*scaled + min;
}


#endif /* __GENETIC_H__ */