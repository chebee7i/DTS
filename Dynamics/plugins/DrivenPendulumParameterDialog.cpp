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
 
#include "DrivenPendulumParameterDialog.h"
#include "GLMotif/WidgetFactory.h"
#include "IntegrationStepSize.h"
#include "VruiStreamManip.h"

GLMotif::PopupWindow* DrivenPendulumParameterDialog::createDialog()
{
  WidgetFactory factory;
  GLMotif::PopupWindow* parameterDialogPopup=factory.createPopupWindow("ParameterDialogPopup", "DrivenPendulum Parameters");

  GLMotif::RowColumn* parameterDialog=factory.createRowColumn("ParameterDialog", 3);
  factory.setLayout(parameterDialog);

  factory.createLabel("GParameterLabel", "g");

  currentGValue=factory.createTextField("CurrentGValue", 10);
  currentGValue->setString(".3");

  gParameterSlider=factory.createSlider("GParameterSlider", 15.0);
  gParameterSlider->setValueRange(0.0, 2.0, .01);
  gParameterSlider->setValue(.3);
  gParameterSlider->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::sliderCallback);

  factory.createLabel("AParameterLabel", "a");

  currentAValue=factory.createTextField("CurrentAValue", 10);
  currentAValue->setString("4.0");

  aParameterSlider=factory.createSlider("AParameterSlider", 15.0);
  aParameterSlider->setValueRange(0, 8, .01);
  aParameterSlider->setValue(4.0);
  aParameterSlider->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::sliderCallback);

  factory.createLabel("FParameterLabel", "F");

  currentFValue=factory.createTextField("CurrentFValue", 10);
  currentFValue->setString("4.0");

  FParameterSlider=factory.createSlider("FParameterSlider", 15.0);
  FParameterSlider->setValueRange(0, 8, .01);
  FParameterSlider->setValue(4.0);
  FParameterSlider->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::sliderCallback);

  factory.createLabel("WParameterLabel", "w");

  currentWValue=factory.createTextField("CurrentWValue", 10);
  currentWValue->setString(".6");

  wParameterSlider=factory.createSlider("WParameterSlider", 15.0);
  wParameterSlider->setValueRange(0, 6.2831853071795862, .01);
  wParameterSlider->setValue(.6);
  wParameterSlider->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::sliderCallback);


  factory.createLabel("StepSizeLabel", "step size");
  stepSizeValue=factory.createTextField("StepSizeValue", 10);
  double step_size = IntegrationStepSize::instance()->getSavedValue("Driven Pendulum");
  if (step_size > 0.0) stepSizeValue->setString(toString(step_size).c_str());
  else stepSizeValue->setString("0.01");
  stepSizeSlider=factory.createSlider("StepSizeSlider", 15.0);
  stepSizeSlider->setValueRange(0.0001, 0.05, 0.0001);
  if (step_size > 0.0) stepSizeSlider->setValue(step_size);
  else stepSizeSlider->setValue(0.01);
  stepSizeSlider->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::sliderCallback);


  factory.createLabel("EvaluationLabel", "Evaluation Method");
  GLMotif::ToggleButton* exactEvalToggle=factory.createCheckBox("ExactEvalToggle", "Exact", true);
  GLMotif::ToggleButton* gridEvalToggle=factory.createCheckBox("GridEvalToggle", "Interpolated Grid");
  // assign line style toggle callbacks
  exactEvalToggle->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::evalTogglesCallback);
  gridEvalToggle->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::evalTogglesCallback);

  factory.createLabel("xSpacingLabel", "x-Grid Spacing");
  currentXValue=factory.createTextField("xTextField", 12);
  currentXValue->setString("1.0");
  currentXValue->setCharWidth(5);
  currentXValue->setPrecision(5);
  xSpacingSlider=factory.createSlider("XSpacingSlider", 15.0);
  xSpacingSlider->setValueRange(.001, 2.0, 0.001);
  xSpacingSlider->setValue(1.0);
  xSpacingSlider->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::sliderCallback);

  factory.createLabel("ySpacingLabel", "y-Grid Spacing");
  currentYValue=factory.createTextField("yTextField", 12);
  currentYValue->setString("1.0");
  currentYValue->setCharWidth(5);
  currentYValue->setPrecision(5);  ySpacingSlider=factory.createSlider("YSpacingSlider", 15.0);
  ySpacingSlider->setValueRange(.001, 2.0, 0.001);
  ySpacingSlider->setValue(1.0);
  ySpacingSlider->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::sliderCallback);

  factory.createLabel("zSpacingLabel", "z-Grid Spacing");
  currentZValue=factory.createTextField("zTextField", 12);
  currentZValue->setString("1.0");
  currentZValue->setCharWidth(5);
  currentZValue->setPrecision(5);
  zSpacingSlider=factory.createSlider("ZSpacingSlider", 15.0);
  zSpacingSlider->setValueRange(.001, 2.0, 0.001);
  zSpacingSlider->setValue(1.0);
  zSpacingSlider->getValueChangedCallbacks().add(this, &DrivenPendulumParameterDialog::sliderCallback);

  // add toggles to array for radio-button behavior
  evalToggles.push_back(exactEvalToggle);
  evalToggles.push_back(gridEvalToggle);

  parameterDialog->manageChild();
  return parameterDialogPopup;
}

void DrivenPendulumParameterDialog::sliderCallback(GLMotif::Slider::ValueChangedCallbackData* cbData)
{
  double value = cbData->value;

  char buff[10];
  snprintf(buff, sizeof(buff), "%3.2f", value);

  if (strcmp(cbData->slider->getName(), "GParameterSlider")==0)
    {
      currentGValue->setString(buff);
      model->setValue("g", value);
    }
  else if (strcmp(cbData->slider->getName(), "AParameterSlider")==0)
    {
      currentAValue->setString(buff);
      model->setValue("a", value);
    }
  else if (strcmp(cbData->slider->getName(), "FParameterSlider")==0)
    {
      currentFValue->setString(buff);
      model->setValue("F", value);
    }
  else if (strcmp(cbData->slider->getName(), "WParameterSlider")==0)
    {
      currentWValue->setString(buff);
      model->setValue("w", value);
    }
  else if (strcmp(cbData->slider->getName(), "StepSizeSlider")==0)
  {
    snprintf(buff, sizeof(buff), "%6.4f", value);
    stepSizeValue->setString(buff);
    IntegrationStepSize::instance()->setValue(value);
    IntegrationStepSize::instance()->saveValue("Driven Pendulum", value);
  }

  else if (strcmp(cbData->slider->getName(), "XSpacingSlider")==0)
    {
      snprintf(buff, sizeof(buff), "%3.3f", value);
      currentXValue->setString(buff);
      model->setSpacing(0, value);
    }
  else if (strcmp(cbData->slider->getName(), "YSpacingSlider")==0)
    {
      snprintf(buff, sizeof(buff), "%3.3f", value);
      currentYValue->setString(buff);
      model->setSpacing(1, value);
    }
  else if (strcmp(cbData->slider->getName(), "ZSpacingSlider")==0)
    {
      snprintf(buff, sizeof(buff), "%3.3f", value);
      currentZValue->setString(buff);
      model->setSpacing(2, value);
    }
}

void DrivenPendulumParameterDialog::evalTogglesCallback(GLMotif::ToggleButton::ValueChangedCallbackData* cbData)
{
   std::string name=cbData->toggle->getName();

   if (name == "ExactEvalToggle")
   {
      model->setSimulate(false);
   }
   else if (name == "GridEvalToggle")
   {
      model->setSimulate(true);
   }

   // fake radio-button behavior
   for (ToggleArray::iterator button=evalToggles.begin(); button != evalToggles.end(); ++button)
      if (strcmp((*button)->getName(), name.c_str()) != 0 and (*button)->getToggle())
         (*button)->setToggle(false);
      else if (strcmp((*button)->getName(), name.c_str()) == 0)
         (*button)->setToggle(true);

}

