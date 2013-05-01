/*****************************************************************************
       Copyright (c) 2002-2013 by John Oliva - All Rights Reserved
*****************************************************************************
  File:         AdaptOrg.h
  Purpose:      Declaration for the AdaptOrg class library.
*****************************************************************************/

#ifndef __ADAPTORGH__
#define __ADAPTORGH__

#include <math.h>
#include <string>
#include <fstream>

#include "AdaptAI.h"

using namespace AdaptAI;

namespace AdaptOrg {
   class Organism {
      protected:
         class State {
            public:
               std::string Name;

               State  ();
               State  (const State &V);
               ~State ();

               bool SetName (std::string NewName);

               bool Save (std::fstream &File);
               bool Load (std::fstream &File);

               State &operator = (const State &S);
         } *States;

         class Sensor {
            public:
               std::string Name;
               float Value;

               Sensor  ();
               Sensor  (const Sensor &S);
               ~Sensor ();

               bool SetName  (std::string NewName);
               bool SetValue (float V);

               bool Save (std::fstream &File);
               bool Load (std::fstream &File);

               Sensor &operator = (const Sensor &S);
         } *Sensors;

         int StateCount, SensorCount, CurrentState;

         Genome OrgGenome;

         bool Free ();

      public:
         Organism  ();
         Organism (const Organism &Org);
         ~Organism ();

         Organism &operator = (const Organism &Org);
         Organism operator  + (const Organism &Org) const;

         std::string GetStateName (int Index) const;
         bool  GetStateName (int Index, std::string* Name) const;
         bool  SetStateName (int Index, std::string Name);

         bool  SetStateCount (int Count);
         int   GetStateCount () const;

         int   GetStateIndex (std::string Name) const;

         float GetSensorValue (std::string Name) const;
         float GetSensorValue (int Index) const;
         bool  SetSensorValue (std::string Name, float Value);
         bool  SetSensorValue (int Index, float Value);

         std::string GetSensorName (int Index) const;
         bool  GetSensorName (int Index, std::string* Name) const;
         bool  SetSensorName (int Index, std::string Name);

         bool  SetSensorCount (int Count);
         int   GetSensorCount () const;

         int   GetSensorIndex (std::string Name) const;

         bool SetTransition (int Index1, int Index2, float BaseChance, const float *SensorCoeff);

         int  GetCurrentState () const;
         bool GetCurrentState (std::string* Name) const;
         bool SetCurrentState (std::string Name);
         bool SetCurrentState (int Index);

         bool Mutate ();
         bool MutateMutationFactors (float Chance, float Rate);

         bool UpdateState ();

         bool Save (std::fstream &File) const;
         bool Load (std::fstream &File);
   };
}

#endif

