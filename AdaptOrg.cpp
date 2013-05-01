/*****************************************************************************
       Copyright (c) 2002-2013 by John Oliva - All Rights Reserved
*****************************************************************************
  File:         AdaptOrg.cpp
  Purpose:      Implementation for the AdaptOrg class library.
*****************************************************************************/

#include "AdaptOrg.h"

using namespace AdaptOrg;

Organism::State::State () {
}

Organism::State::State (const State &V) {
   (*this) = V;
}

Organism::State::~State () {
}

bool Organism::State::SetName (std::string NewName) {
   Name = NewName;
   return true;
}

bool Organism::State::Save (std::fstream &File) {
   // File format:
   //              Count         sizeof (int)
   //              Name          sizeof (char) * Count

   int Count = Name.size() + 1;

   File.write ((const char *) &Count, sizeof (int));
   File.write ((const char *) Name.c_str(), sizeof (char) * Count);

   if (!File.good ())
      return false;

   return true;
}

bool Organism::State::Load (std::fstream &File) {
   int Count;

   File.read ((char *) &Count, sizeof (int));

   char *cname = new char [Count];
   File.read ((char *) cname, sizeof (char) * Count);

	Name = cname;

	delete [] cname;

   if (!File.good ())
      return false;

   return true;
}

Organism::State &Organism::State::operator = (const State &V) {
	Name = V.Name;
   return *this;
}

Organism::Sensor::Sensor () {
   Value = 0.0F;
}

Organism::Sensor::Sensor (const Sensor &S) {
   (*this) = S;
}

Organism::Sensor::~Sensor () {
}

bool Organism::Sensor::Save (std::fstream &File) {
   // File format:
   //              Count         sizeof (int)
   //              Value         sizeof (float)
   //              Name          sizeof (char) * Count

   int Count = Name.size() + 1;

   File.write ((const char *) &Count, sizeof (int));
   File.write ((const char *) &Value, sizeof (float));
   File.write ((const char *) Name.c_str(), sizeof (char) * Count);

   if (!File.good ())
      return false;

   return true;
}

bool Organism::Sensor::Load (std::fstream &File) {
   int Count;

   File.read ((char *) &Count, sizeof (int));
   File.read ((char *) &Value, sizeof (float));

   char *cname = new char [Count];

   File.read ((char *) cname, sizeof (char) * Count);
	Name = cname;

	delete [] cname;

   if (!File.good ())
      return false;

   return true;
}

bool Organism::Sensor::SetName  (std::string NewName) {
	Name = NewName;
   return true;
}

bool Organism::Sensor::SetValue (float V) {
   Value = V;

   return true;
}

Organism::Sensor &Organism::Sensor::operator = (const Sensor &S) {
	Name = S.Name;
   Value = S.Value;
   return *this;
}

bool Organism::Free () {
   delete [] States;

   States = NULL;

   StateCount = 0;

   delete [] Sensors;

   Sensors = NULL;

   SensorCount = 0;

   CurrentState = 0;

   return true;
}

Organism::Organism  () {
   States = NULL;
   Sensors   = NULL;

   StateCount = SensorCount = CurrentState = 0;
}

Organism::Organism (const Organism &Org) {
   (*this) = Org;
}

Organism::~Organism () {
   Free ();
}

Organism &Organism::operator = (const Organism &Org) {
   Free ();

   StateCount  = Org.StateCount;
   SensorCount = Org.SensorCount;

   States  = new State  [StateCount];
   Sensors = new Sensor [SensorCount];

   int i;

   for (i = 0; i < StateCount; i++)
      States [i] = Org.States [i];

   for (i = 0; i < SensorCount; i++)
      Sensors [i] = Org.Sensors [i];

   OrgGenome = Org.OrgGenome;

   return *this;
}

Organism Organism::operator + (const Organism &Org) const {
   Organism Temp;

   if (StateCount != Org.StateCount || SensorCount != Org.SensorCount) {
      throw;
   }

   Temp = *this;

   Temp.OrgGenome = OrgGenome + Org.OrgGenome;

   // Mutate the offspring's genome:
   Temp.Mutate ();

   return Temp;
}

std::string Organism::GetStateName (int Index) const {
   if (Index < 0 || Index >= StateCount)
      return NULL;

   return States [Index].Name;
}

bool Organism::GetStateName (int Index, std::string* Name) const {
   if (Index < 0 || Index >= StateCount)
      return false;

	*Name = States [Index].Name;

   return true;
}

bool Organism::SetStateName (int Index, std::string Name) {
   if (Index < 0 || Index >= StateCount)
      return false;

   States [Index].SetName (Name);

   return true;
}

bool Organism::SetStateCount (int Count) {
   if (Count < 0)
      return false;

   // One chromosome for each state:
   StateCount = Count;

   if (States == NULL) {
      States = new State [StateCount];
   }

   if (!OrgGenome.SetChromosomeCount (StateCount))
      return false;

   // StateCount genes for each state:
   for (int i = 0; i < StateCount; i++) {
      Chromosome &Chrom = OrgGenome.GetChromosome (i);

      Chrom.SetGeneCount (StateCount);
   }

   // Now set sensor count since it depends on state count:
   return SetSensorCount (SensorCount);
}

int Organism::GetStateCount () const {
   return StateCount;
}

int Organism::GetStateIndex (std::string Name) const {
   for (int i = 0; i < StateCount; i++) {
      if (Name == States [i].Name) {
         return i;
      }
   }

   return -1;
}

float Organism::GetSensorValue (std::string Name) const {
   for (int i = 0; i < SensorCount; i++) {
      if (Name == Sensors [i].Name)
         return Sensors [i].Value;
   }

   return 0.0F;
}

float Organism::GetSensorValue (int Index) const {
   if (Index < 0 || Index >= SensorCount)
      return 0.0F;

   return Sensors [Index].Value;
}

bool Organism::SetSensorValue (std::string Name, float Value) {
   for (int i = 0; i < SensorCount; i++) {
      if (Name == Sensors [i].Name) {
         Sensors [i].SetValue (Value);

         return true;
      }
   }

   return false;
}

bool Organism::SetSensorValue (int Index, float Value) {
   if (Index < 0 || Index >= SensorCount)
      return false;

   Sensors [Index].SetValue (Value);

   return true;
}

std::string Organism::GetSensorName (int Index) const {
   if (Index < 0 || Index >= SensorCount)
      return NULL;

   return Sensors [Index].Name;
}

bool Organism::GetSensorName (int Index, std::string* Name) const {
   if (Index < 0 || Index >= SensorCount)
      return false;

	*Name = Sensors [Index].Name;

   return true;
}

bool Organism::SetSensorName (int Index, std::string Name) {
   if (Index < 0 || Index >= SensorCount)
      return false;

   Sensors [Index].SetName (Name);

   return true;
}

bool Organism::SetSensorCount (int Count) {

   if (Count < 0) {
      return false;
   }
   
   SensorCount = Count;

   if (Count > 0) {
      if (Sensors == NULL) {
         Sensors = new Sensor [SensorCount];
      }
   }

   for (int i = 0; i < StateCount; i++) {
      Chromosome &Chrom = OrgGenome.GetChromosome (i);

      for (int j = 0; j < StateCount; j++) {
         Gene &G = Chrom.GetGene (j);

         // Allocate enough room for each coefficient
         // (base + sensor coeff's):
         G.SetLength (1 + SensorCount);
      }
   }

   return true;
}

int Organism::GetSensorCount () const {
   return SensorCount;
}

int Organism::GetSensorIndex (std::string Name) const {
   for (int i = 0; i < SensorCount; i++) {
      if (Name == Sensors [i].Name) {
         return i;
      }
   }

   return -1;
}

bool Organism::SetTransition (int Index1, int Index2, float BaseChance, const float *SensorCoeff) {
   if (Index1 < 0 || Index1 >= StateCount || Index2 < 0 || Index2 >= StateCount)
      return false;

   Chromosome &Chrom = OrgGenome.GetChromosome (Index1);

   Gene &G = Chrom.GetGene (Index2);

   G.SetElement (0, BaseChance);

   for (int i = 0; i < SensorCount; i++)
      G.SetElement (1 + i, SensorCoeff [i]);

   return true;
}

int Organism::GetCurrentState () const {
   return CurrentState;
}

bool Organism::GetCurrentState (std::string* Name) const {
   *Name = States [CurrentState].Name;
   return true;
}

bool Organism::SetCurrentState (std::string Name) {
   for (int i = 0; i < StateCount; i++) {
		if (States [i].Name == Name) {
         CurrentState = i;

         return true;
      }
   }

   return false;
}

bool Organism::SetCurrentState (int Index) {
   if (Index < 0 || Index >= StateCount)
      return false;

   CurrentState = Index;

   return true;
}

bool Organism::Mutate () {
   return OrgGenome.Mutate ();
}

bool Organism::MutateMutationFactors (float Chance, float Rate) {
   return OrgGenome.MutateMutationFactors (Chance, Rate);
}

bool Organism::UpdateState () {
   float *Prob = new float [StateCount];

   float TotalProb = 0.0F;

   int i;

   Chromosome &Chrom = OrgGenome.GetChromosome (CurrentState);

   for (i = 0; i < StateCount; i++) {
      Gene &G = Chrom.GetGene (i);

      // Base chance:
      Prob [i]  = G.GetElement (0);

      TotalProb  += Prob [i];

      // Sensor coefficients:
      for (int j = 1; j <= SensorCount; j++) {
         float p = G.GetElement (j) * Sensors [j - 1].Value;
         
         Prob [i] += p;

         TotalProb  += p;
      }
   }

   // Normalize probability:
   for (i = 0; i < StateCount; i++)
      Prob [i] /= TotalProb;

   // cdf - cumulative distribution function
   for (i = StateCount - 1; i >= 0; i--) {
      for (int j = 0; j < i; j++)
         Prob [i] += Prob [j];
   }

   float Choice = Random ();

   int NextState = CurrentState;

   for (i = 0; i < StateCount; i++) {
      if (Choice <= Prob [i]) {
         NextState = i;

         break;
      }
   }

   CurrentState = NextState;

   delete [] Prob;

   return true;
}

bool Organism::Save (std::fstream &File) const {
   // File format:
   //          StateCount         sizeof (int)
   //          SensorCount        sizeof (int)
   //          CurrentState       sizeof (int)
   //          States             sizeof (State) * StateCount
   //          Sensors            sizeof (Sensor) * SensorCount
   //          OrgGenome          varies
   File.write ((const char *) &StateCount,   sizeof (int));
   File.write ((const char *) &SensorCount,  sizeof (int));
   File.write ((const char *) &CurrentState, sizeof (int));
   
   // Save states:
   int i;
   for (i = 0; i < StateCount; i++) {
      if (!States [i].Save (File))
         return false;
   }

   // Save sensors:
   for (i = 0; i < SensorCount; i++) {
      if (!Sensors [i].Save (File))
         return false;
   }

   // Save the genome:
   if (!OrgGenome.Save (File))
      return false;

   if (!File.good ())
      return false;

   return true;
}

bool Organism::Load (std::fstream &File) {
   File.read ((char *) &StateCount,   sizeof (int));
   File.read ((char *) &SensorCount,  sizeof (int));
   File.read ((char *) &CurrentState, sizeof (int));
   
   // Allocate memory for states:
   delete [] States;
   States = new State [StateCount];

   // Load states:
   int i;
   for (i = 0; i < StateCount; i++) {
      if (!States [i].Load (File))
         return false;
   }

   // Allocate memory for the sensors:
   delete [] Sensors;
   Sensors = new Sensor [SensorCount];

   // Load sensors:
   for (i = 0; i < SensorCount; i++) {
      if (!Sensors [i].Load (File))
         return false;
   }

   // Load the genome:
   if (!OrgGenome.Load (File))
      return false;

   if (!File.good ())
      return false;

   return true;
}

