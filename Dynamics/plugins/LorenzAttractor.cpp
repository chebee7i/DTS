/*
 * DO NOT EDIT THIS FILE.
 *
 * THIS FILE HAS BEEN AUTOMATICALLY GENERATED.
 * ANY CHANGES MADE TO THE CODE WILL BE LOST.
 *
 * TO MODIFY THE PARAMETER VALUES OR DYNAMICAL
 * EQUATIONS EDIT THE XML FILE (dynamics.xml).
 * 
 */
 
#include "LorenzAttractor.h"
  Integrator* maker()
  {
    return new LorenzAttractorIntegrator;
  }

  class LorenzAttractorProxy
  {
    public:
      LorenzAttractorProxy()
      {
        Factory["Lorenz Attractor"] = maker;
      }
  };

  LorenzAttractorProxy p;
