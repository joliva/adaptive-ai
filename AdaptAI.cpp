/*****************************************************************************
       Copyright (c) 2002-2013 by John Oliva - All Rights Reserved
*****************************************************************************
  File:         AdaptAI.cpp
  Purpose:      Implementation for the AdaptAI class library.
*****************************************************************************/

#include "AdaptAI.h"

using namespace AdaptAI;

//
// General functions
//

float AdaptAI::Random () {
   return rand () / (float) RAND_MAX;
}

//
// Gene implementation
//

Gene::Gene () {
   Sequence = NULL;

   MutationChance = ADAPTAI_DEFAULTCHANCE;

   MutationRate   = ADAPTAI_DEFAULTRATE;

   SequenceLength = 0;
}

Gene::Gene (const Gene &Gene) {
   (*this) = Gene;
}

Gene::~Gene () {
   delete [] Sequence;
}

bool Gene::SetElement (int i, float El) {
   if (i >= 0 && i < SequenceLength) {
      Sequence [i] = El;

      return true;
   }

   return false;
}

float Gene::GetElement (int i) const {
   if (i >= 0 && i < SequenceLength) {
      return Sequence [i];
   }

   return 0.0F;
}

bool Gene::SetLength (int Length) {
   delete [] Sequence;

   SequenceLength = Length;

   Sequence = new float [SequenceLength];

   // Initialize to zero:
   for (int i = 0; i < SequenceLength; i++)
      Sequence [i] = 0.0F;

   return true;
}

int Gene::GetLength () const {
   return SequenceLength;
}

bool Gene::SetMutationChance (float Chance) {
   // Crop to [0, 1]
   if (Chance < 0.0F)
      Chance = 0.0F;
   if (Chance > 1.0F)
      Chance = 1.0F;

   MutationChance = Chance;

   return true;
}

float Gene::GetMutationChance () const {
   return MutationChance;
}

bool Gene::SetMutationRate (float MR) {
   MutationRate = MR;

   return true;
}

float Gene::GetMutationRate () const {
   return MutationRate;
}

bool Gene::Mutate () {
   if (SequenceLength <= 0)
      return false;

   for (int i = 0; i < SequenceLength; i++) {
      if (Random () <= MutationChance)
         Sequence [i] = Sequence [i] + (2.0F * Random () - 1.0F) * MutationRate;
   }

   return true;
}

bool Gene::MutateMutationFactors (float Chance, float Rate) {
   if (Random () <= Chance) {
      MutationChance += (Random () * 2.0F - 1.0F) * Rate;
      MutationRate   += (Random () * 2.0F - 1.0F) * Rate;
   }

   return true;
}

Gene &Gene::operator = (const Gene &G) {
   SetLength (G.SequenceLength);

   MutationChance = G.MutationChance;
   MutationRate   = G.MutationRate;

   for (int i = 0; i < SequenceLength; i++)
      Sequence [i] = G.Sequence [i];

   return *this;
}

Gene Gene::operator + (const Gene &G) const {
   Gene NewG;

   if (SequenceLength != G.SequenceLength)
      return NewG;

   NewG.SetLength (SequenceLength);

   // Arithmetic average of elements:
   for (int i = 0; i < SequenceLength; i++) {
      NewG.Sequence [i] = (Sequence [i] + G.Sequence [i]) / 2.0F;
   }

   return NewG;
}

bool Gene::Save (std::fstream &File) const {
   // File Format:
   //          SequenceLength       sizeof (int)
   //          MutationChance       sizeof (float)
   //          MutationRate         sizeof (float)
   //          Sequence             sizeof (float) * SequenceLength
   File.write ((const char *) &SequenceLength, sizeof (int));
   File.write ((const char *) &MutationChance, sizeof (float));
   File.write ((const char *) &MutationRate,   sizeof (float));
   File.write ((const char *) Sequence,        sizeof (float) * SequenceLength);

   if (!File.good ())
      return false;

   return true;
}

bool Gene::Load (std::fstream &File) {
   File.read ((char *) &SequenceLength, sizeof (int));
   File.read ((char *) &MutationChance, sizeof (float));
   File.read ((char *) &MutationRate,   sizeof (float));

   if (!SetLength (SequenceLength))
      return false;

   File.read ((char *) Sequence, sizeof (float) * SequenceLength);

   if (!File.good ())
      return false;

   return true;   
}

//
// Chromosome implementation
//

Chromosome::Chromosome () {
   GeneList = NULL;

   GeneCount = 0;

   Crossover = true;

   CrossoverMutationChance = ADAPTAI_DEFAULTCHANCE;
}

Chromosome::Chromosome (const Chromosome &Chrom) {
   (*this) = Chrom;
}

Chromosome::~Chromosome () {
   delete [] GeneList;
}

bool Chromosome::SetGene (int i, const Gene &G) {
   if (i < 0 || i >= GeneCount)
      return false;

   GeneList [i] = G;

   return true;
}

Gene &Chromosome::GetGene (int i) const {
   if (i < 0 || i >= GeneCount) {
      throw;
   }
   
   return *(&GeneList [i]);
}

bool Chromosome::SetGeneCount (int Length) {
   if (Length < 0)
      return false;

   delete [] GeneList;

   GeneCount = Length;

   GeneList  = new Gene [GeneCount];

   return true;
}

int Chromosome::GetGeneCount () const {
   return GeneCount;
}

Chromosome &Chromosome::operator = (const Chromosome &Chrom) {
   SetGeneCount (Chrom.GeneCount);

   Crossover               = Chrom.Crossover;
   CrossoverMutationChance = Chrom.CrossoverMutationChance;

   // Copy gene info:
   for (int i = 0; i < GeneCount; i++) {
      GeneList [i] = Chrom.GeneList [i];
   } 

   return *this;
}

Chromosome Chromosome::operator + (const Chromosome &Chrom) const {
   Chromosome Temp;

   if (GeneCount != Chrom.GeneCount)
      return Temp;

   Temp.SetGeneCount (GeneCount);

   if (Crossover) {
      // 50% chance of inheriting crossover trait & mutation rate from either parent:
      if (Random () < 0.5F) {
         Temp.Crossover               = Crossover;
         Temp.CrossoverMutationChance = CrossoverMutationChance;
      }
      else {
         Temp.Crossover               = Chrom.Crossover;
         Temp.CrossoverMutationChance = Chrom.CrossoverMutationChance;
      }

      for (int i = 0; i < GeneCount; i++) {
         // 50% chance of inheriting gene from either parent:
         if (Random () < 0.5F)
            Temp.GeneList [i] = GeneList [i];
         else Temp.GeneList [i] = Chrom.GeneList [i];
      } 
   }
   else {
      // 50% chance of inheriting crossover trait from either parent:
      if (Random () < 0.5F)
         Temp.Crossover = Crossover;
      else Temp.Crossover = Chrom.Crossover;

      // Mutation chance is numerical average of parents:
      Temp.CrossoverMutationChance = (CrossoverMutationChance + Chrom.CrossoverMutationChance) / 2.0F;

      // Genes are numerical average of parents:
      for (int i = 0; i < GeneCount; i++) {
         Temp.GeneList [i] = GeneList [i] + Chrom.GeneList [i];
      } 
   }

   return Temp;
}

bool Chromosome::SetCrossoverState (bool State) {
   Crossover = State;

   return true;
}

bool Chromosome::GetCrossoverState () {
   return Crossover;
}

bool Chromosome::SetCrossoverMutationChance (float C) {
   // Clamp mutation chance:
   if (C < 0.0F)
      C = 0.0F;
   else if (C > 1.0F)
      C = 1.0F;

   CrossoverMutationChance = C;

   return true;
}

float Chromosome::GetCrossoverMutationChance () {
   return CrossoverMutationChance;
}

bool Chromosome::MutateChromosome () {
   if (Random () <= CrossoverMutationChance) {
      if (Crossover) {
         Crossover = false;
      }
      else {
         Crossover = true;
      }
   }

   return true;
}

bool Chromosome::MutateGenes () {
   for (int i = 0; i < GeneCount; i++)
      GeneList [i].Mutate ();

   return true;
}

bool Chromosome::Mutate () {
   return (MutateChromosome () && MutateGenes ());
}

bool Chromosome::MutateMutationFactors (float Chance, float Rate) {
   if (Random () <= Chance) {
      SetCrossoverMutationChance (CrossoverMutationChance + (Random () * 2.0F - 1.0F) * Rate);
   }

   for (int i = 0; i < GeneCount; i++) {
      GeneList [i].MutateMutationFactors (Chance, Rate);
   }

   return true;
}

bool Chromosome::Save (std::fstream &File) const {
   // File Format:
   //          GeneCount               sizeof (int)
   //          Crossover               sizeof (bool)
   //          CrossoverMutationChance sizeof (float)
   //          GeneList                varies

   File.write ((const char *) &GeneCount, sizeof (int));
   File.write ((const char *) &Crossover, sizeof (bool));
   File.write ((const char *) &CrossoverMutationChance, sizeof (bool));

   for (int i = 0; i < GeneCount; i++)
      GeneList [i].Save (File);

   if (!File.good ())
      return false;

   return true;
}

bool Chromosome::Load (std::fstream &File) {
   File.read ((char *) &GeneCount, sizeof (int));
   File.read ((char *) &Crossover, sizeof (bool));
   File.read ((char *) &CrossoverMutationChance, sizeof (float));

   SetGeneCount (GeneCount);

   for (int i = 0; i < GeneCount; i++)
      GeneList [i].Load (File);

   if (!File.good ())
      return false;

   return true;
}

//
// Genome implementation
//

Genome::Genome () {
   ChromosomeList  = NULL;
   ChromosomeCount = 0;
}

Genome::Genome (const Genome &G) {
   (*this) = G;
}

Genome::~Genome () {
   delete [] ChromosomeList;
}

bool Genome::SetChromosome (int i, const Chromosome &Chrom) {
   if (i < 0 || i >= ChromosomeCount)
      return false;

   ChromosomeList [i] = Chrom;

   return true;
}

Chromosome &Genome::GetChromosome (int i) const {
   if (i < 0 || i >= ChromosomeCount) {
      throw;
   }

   return *(&ChromosomeList [i]);
}

bool Genome::SetChromosomeCount (int Count) {
   if (Count < 0)
      return false;

   delete [] ChromosomeList;

   ChromosomeCount = Count;

   ChromosomeList = new Chromosome [ChromosomeCount];

   return true;   
}

int Genome::GetChromosomeCount () const {
   return ChromosomeCount;
}

Genome &Genome::operator = (const Genome &G) {
   ChromosomeCount = G.ChromosomeCount;

   SetChromosomeCount (ChromosomeCount);

   for (int i = 0; i < ChromosomeCount; i++)
      ChromosomeList [i] = G.ChromosomeList [i];

   return *this;
}

Genome Genome::operator + (const Genome &G) const {
   Genome Temp;

   if (ChromosomeCount != G.ChromosomeCount) {
      throw;
   }

   Temp.SetChromosomeCount (ChromosomeCount);

   for (int i = 0; i < ChromosomeCount; i++)
      Temp.ChromosomeList [i] = ChromosomeList [i] + G.ChromosomeList [i];

   return Temp;
}

bool Genome::Mutate () {
   for (int i = 0; i < ChromosomeCount; i++) {
      ChromosomeList [i].Mutate ();
   }

   return true;
}

bool Genome::MutateMutationFactors (float Chance, float Rate) {
   for (int i = 0; i < ChromosomeCount; i++) {
      ChromosomeList [i].MutateMutationFactors (Chance, Rate);
   }

   return true;
}

bool Genome::Save (std::fstream &File) const {
   // File Format:
   //          ChromosomeCount       sizeof (int)
   //          ChromosomeList        varies

   File.write ((const char *) &ChromosomeCount, sizeof (int));

   for (int i = 0; i < ChromosomeCount; i++)
      ChromosomeList [i].Save (File);

   if (!File.good ())
      return false;

   return true;
}

bool Genome::Load (std::fstream &File) {
   // File Format:
   //          ChromosomeCount       sizeof (int)
   //          ChromosomeList        varies

   File.read ((char *) &ChromosomeCount, sizeof (int));

   SetChromosomeCount (ChromosomeCount);

   for (int i = 0; i < ChromosomeCount; i++)
      ChromosomeList [i].Load (File);

   if (!File.good ())
      return false;

   return true;   
}

