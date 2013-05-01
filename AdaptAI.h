/*****************************************************************************
       Copyright (c) 2002-2013 by John Oliva - All Rights Reserved
*****************************************************************************
  File:         AdaptAI.h
  Purpose:      Declaration for the AdaptAI class library.
*****************************************************************************/

#ifndef __ADAPTAIH__
#define __ADAPTAIH__

#ifndef NULL
#define NULL 0
#endif

#define ADAPTAI_DEFAULTCHANCE 0.001F
#define ADAPTAI_DEFAULTRATE   0.1F

#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>

namespace AdaptAI {

   class Gene {
      protected:
         float *Sequence, MutationChance, MutationRate;

         int SequenceLength;

      public:
         Gene  ();      
         Gene  (const Gene &Gene);
         ~Gene ();

         bool  SetElement (int i, float El);
         float GetElement (int i) const;

         bool SetLength (int Length);
         int  GetLength () const;

         bool  SetMutationChance (float Chance);
         float GetMutationChance () const;

         bool  SetMutationRate (float MR);
         float GetMutationRate () const;

         bool Mutate ();

         bool MutateMutationFactors (float Chance, float Rate);

         Gene &operator = (const Gene &G);
         Gene operator  + (const Gene &G) const;

         bool Save (std::fstream &File) const;
         bool Load (std::fstream &File);
   };

   class Chromosome {
      protected:
         Gene *GeneList;

         int GeneCount;

         bool  Crossover;
         float CrossoverMutationChance;

      public:
         Chromosome  ();
         Chromosome  (const Chromosome &Chrom);
         ~Chromosome ();

         bool SetGene  (int i, const Gene &G);
         Gene &GetGene (int i) const;

         bool SetGeneCount (int Length);
         int  GetGeneCount () const;

         Chromosome &operator = (const Chromosome &Chrom);
         Chromosome operator  + (const Chromosome &Chrom) const;

         bool SetCrossoverState (bool State);
         bool GetCrossoverState ();

         bool  SetCrossoverMutationChance (float C);
         float GetCrossoverMutationChance ();

         bool MutateChromosome ();
         bool MutateGenes      ();
         bool Mutate ();

         bool MutateMutationFactors (float Chance, float Rate);

         bool Save (std::fstream &File) const;
         bool Load (std::fstream &File);
   };

   class Genome {
      protected:
         Chromosome *ChromosomeList;

         int ChromosomeCount;

      public:
         Genome  ();
         Genome (const Genome &G);
         ~Genome ();

         bool       SetChromosome  (int i, const Chromosome &Chrom);
         Chromosome &GetChromosome (int i) const;

         bool SetChromosomeCount (int Count);
         int  GetChromosomeCount () const;

         Genome &operator = (const Genome &G);
         Genome operator  + (const Genome &G) const;

         bool Mutate ();

         bool MutateMutationFactors (float Chance, float Rate);

         bool Save (std::fstream &File) const;
         bool Load (std::fstream &File);        
   };

   extern float Random ();
}

#endif

