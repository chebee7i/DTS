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
 
#ifndef RIKITAKEDYNAMO_INCLUDED_
#define RIKITAKEDYNAMO_INCLUDED_

#include "BaseModel.h"
#include "RungeKutta4.h"

#include "RikitakeDynamoParameterDialog.h"

class RikitakeDynamo : public DynamicalModel
{
  Scalar nu;
  Scalar a;
public:
  RikitakeDynamo(Scalar Nu, Scalar A)
     : nu(Nu), a(A)
  { }

  virtual ~RikitakeDynamo() { }

  virtual Vector exact(const Point& p) const
    {
      return Vector(-nu*p[0]+p[2]*p[1],-nu*p[1]-p[0]*(p[2]-a),1.0-p[0]*p[1]);
    }

private:
  virtual void _setValue(const std::string& name, Scalar value)
    {
      if      (name == "nu") nu = value;
      else if (name == "a") a = value;
    }
};

class RikitakeDynamoIntegrator : public Integrator
{
  RikitakeDynamo* dynamics;
  RungeKutta4<RikitakeDynamo>* integrator;
public:
  RikitakeDynamoIntegrator()
    {
      dynamics = new RikitakeDynamo(0.2, 5.0);
      double stepSize = 0.01;
      integrator = new RungeKutta4<RikitakeDynamo>(*dynamics, stepSize);
    }

  virtual ~RikitakeDynamoIntegrator()
    {
      delete dynamics;
      delete integrator;
    }

  inline Vector step(const Point& p)
    {
      return integrator->step(p);
    }

  virtual unsigned int getModelVersion()
    {
      return dynamics->getModelVersion();
    }

  virtual CaveDialog* createParameterDialog(GLMotif::PopupMenu *parent)
    {
      return new RikitakeDynamoParameterDialog(parent, dynamics);
    }

  virtual Vrui::Point center() const
    {
      return Vrui::Point(0.0, 0.0, 0.0);
    }
};

#endif
