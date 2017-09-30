#include <stdio.h>
#include <genetic.h>
#include <stdlib.h>
#include <string.h>

/* INTERNAL functions */
static void shuffle(uint32_t *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

/* private functions used as PUBLIC methods */
static void priv_print_individual(struct population_s *this, uint32_t individual_id)
{
    uint32_t i = 0;

    for(i=0;i<this->individuals[individual_id].size;i++)
    {
        printf("%.2f ", this->individuals[individual_id].genes[i]);
    }
    printf("Fit:%.2f\n", this->fitness_cb(this, individual_id));
}

static void priv_print_population(struct population_s *this)
{
   int i = 0;

   for(i=0;i<this->count;i++)
   {
      if(i==this->retain_count) printf("------------------------------------\n");
      this->print_individual(this, i);
   }
}

static double priv_get_fitness(struct population_s *this, uint32_t individual_id)
{
    double ret_val = 0;

    if(this->fitness_cb != NULL)
        ret_val = this->fitness_cb(this, individual_id);

    this->individuals[individual_id].fitness_val = ret_val;

    return ret_val;
}

static double priv_grade(struct population_s *this)
{
   uint32_t i = 0;
   double summed = 0;

   for(i=0;i<this->retain_count;i++)
   {
      summed += this->get_fitness(this, i);
   }
   return summed/this->retain_count;
}

static void priv_evolve(struct population_s *this)
{
   uint32_t i = 0;
   uint32_t j = 0;
   uint32_t k = 0;
   uint32_t mother = 0;
   uint32_t father = 0;
   uint32_t random_individual_index=0;
   uint32_t random_gene_index=0;
   double *tmp_genes = NULL;
   uint32_t father_mother_inheritance_level = 0;

   uint32_t *gen_index = malloc(this->individuals[0].size*sizeof(uint32_t));
   for(i=0;i<this->individuals[0].size;i++)
   {
      gen_index[i] = i;
   }

   uint32_t *retain_index = malloc(this->retain_count*sizeof(uint32_t));
   for(i=0;i<this->retain_count;i++)
   {
      retain_index[i] = i;
   }

   tmp_genes = malloc(this->individuals[0].size*sizeof(double));

   /* Sort population */
   for(i=0; i<this->count; i++)
   {
      for(j=0; j<this->count-1; j++)
      {
         if(this->get_fitness(this, j) > this->get_fitness(this,j+1))
         {
            memcpy(tmp_genes, this->individuals[j+1].genes, sizeof(double)*this->individuals[j+1].size);
            memcpy(this->individuals[j+1].genes, this->individuals[j].genes, sizeof(double)*this->individuals[j+1].size);
            memcpy(this->individuals[j].genes, tmp_genes, sizeof(double)*this->individuals[j+1].size);
         }
      }
   }

   free(tmp_genes);

   /* Randomize parents */
   shuffle(retain_index, this->retain_count);

   /* Kill the least ones and make the new ones from the parents */
   for(i=(this->retain_count);i<this->count;i++,k++)
   {
      /* take a 50:50 chance for a loser to become a parent */
      if(((i<(this->retain_count+this->loser_level)) && (i < this->count-2)) && rand_in_range(0, 1))
      {
         mother = rand_in_range(i+1, this->count-1);
         //printf("LOSER! i= %d %d\n", i, mother);
      }
      else
      {
         mother = retain_index[k%this->retain_count];
      }
      father = retain_index[(k+1)%this->retain_count];

      //printf("father: %d mother %d\n", father, mother);

      /* take random genes from mother and father */
      shuffle(gen_index, this->individuals[0].size);
      // printf("Chosen gene order: ");
      // for(k=0;k<this->individuals[0].size;k++)
      // {
      //     printf("%d ", gen_index[k]);
      // }
      // printf("\n");

      /* use random genes from mother and random genes from father */
      /* take at least one gene from father and one from mother */
      //father_mother_inheritance_level = this->individuals[0].size/2;
      father_mother_inheritance_level = rand_in_range(1, (this->individuals[0].size-1));

      for(j=0;j<this->individuals[0].size;j++)
      {
         //printf("father_mother_inheritance_level %d\n", father_mother_inheritance_level);
         #if USE_GENE_MIXING
            if(j<father_mother_inheritance_level)
               this->individuals[i].genes[j] = this->individuals[mother].genes[gen_index[j]];
            else
               this->individuals[i].genes[j] = this->individuals[father].genes[gen_index[j]];
         #else
            if(j<father_mother_inheritance_level)
               this->individuals[i].genes[gen_index[j]] = this->individuals[mother].genes[gen_index[j]];
            else
               this->individuals[i].genes[gen_index[j]] = this->individuals[father].genes[gen_index[j]];
         #endif
      }
   }

   for(i=0;i<this->mutations;i++)
   {
     /* Add some random individual */
     random_individual_index = rand_in_range(this->retain_count, this->count-1);
     random_gene_index = rand_in_range(0, this->individuals[0].size-1);
     this->individuals[random_individual_index].genes[random_gene_index] = this->mutate_cb(this, random_individual_index, random_gene_index);
   }
   
   free(retain_index);
   free(gen_index);
}



/* CONSTRCUCTOR */
population_t *genetic_new(uint32_t count, 
                          uint32_t genom_size, 
                          double retain_val, 
                          uint32_t mutation_level, 
                          uint32_t loser_level, 
                          void *create_individual_cb, 
                          void *fitness_cb, 
                          void *mutate_cb)
{
    uint32_t i = 0;
    double *genes = 0;
    population_t *p_population = NULL;

    /* Create population */
    p_population = malloc(sizeof(population_t));

    /* create population */
    p_population->individuals = malloc(count*sizeof(individual_t));

    for(i=0;i<count;i++)
    {
        /* create individuals */
        genes = malloc(genom_size*sizeof(double));
        memset(genes, 0, genom_size);

        p_population->individuals[i].size = genom_size;
        p_population->individuals[i].fitness_val = 0;
        p_population->individuals[i].genes = genes;
    }

    /* Set method callbacks */
    p_population->print_individual = priv_print_individual;
    p_population->print_population = priv_print_population;
    p_population->get_fitness = priv_get_fitness;
    p_population->grade = priv_grade;
    p_population->evolve = priv_evolve;

    /* Set user callbacks */
    p_population->fitness_cb = fitness_cb;
    p_population->create_individual_cb = create_individual_cb;
    p_population->mutate_cb = mutate_cb;

    /* Set default values */
    p_population->count = count;
    p_population->retain_count = (uint32_t)(count*retain_val);
    p_population->mutations = mutation_level;
    p_population->loser_level = loser_level;


    /* initialize individuals */
    for(i=0;i<p_population->count;i++)
    {
      p_population->create_individual_cb(p_population, i);
    }

    return p_population;
}

/* DESTRUCTOR */
void genetic_free(population_t *this)
{
    uint32_t i = 0;

    /* free allocated individuals */
    for(i=0;i<this->count;i++)
    {
        free(this->individuals[i].genes);
    }
    free(this->individuals);

    /* Now free population */
    free(this);
}
