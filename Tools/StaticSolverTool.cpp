/*******************************************************************************
 StaticSolverTool: Static solver dynamics tool.
 Copyright (c) 2006-2008 Jordan Van Aalsburg

 This file is part of the Dynamics Toolset.

 The Dynamics Toolset is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option) any
 later version.

 The Dynamics Toolset is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License
 along with the Dynamics Toolset. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/
#include "StaticSolverTool.h"

// STL includes
#include <iostream>
#include <vector>


// Vrui includes
//
#include <Geometry/Point.h>
#include <GL/GLPolylineTube.h>

// OpenGL includes
//
#include <GL/glu.h>
#include <GL/gle.h>

//
// StaticSolverData initialization
//

const unsigned int StaticSolverData::MaxPoints=20000;

//
// StaticSolverTool::Icon methods
//

void StaticSolverTool::Icon::display(GLContextData& contextData) const
{
   DataItem* dataItem=contextData.retrieveDataItem<DataItem> (parent);
   glCallList(dataItem->iconDisplayListId);
}

//
// StaticSolverTool methods
//
StaticSolverTool::StaticSolverTool(ToolBox::ToolBox* toolBox, Viewer* app) :
         AbstractDynamicsTool(toolBox, app),
         dataDisplayListVersion(1), // Start higher so displayList is compiled.
         multipleStaticSolutions(false)
      {
         icon(new Icon(this));

         // Set member from parent class
         _needsGLSL = false;
      }

StaticSolverTool::~StaticSolverTool()
{
   clearDatasets();
}

void StaticSolverTool::initContext(GLContextData& contextData) const
{
   DataItem* dataItem=new DataItem;
   contextData.addDataItem(this, dataItem);

   // calculate and store points describing spiral
   const unsigned int SIZE=50;
   gleDouble pts[SIZE][3];
   float clr[SIZE][4];

   BlueRedColorMap colorMap;

   const float total_angle=6.0 * M_PI;

   for (unsigned int i=0; i < SIZE; i++)
   {
      float t=(float) i / (float) SIZE * total_angle;

      pts[i][0]=(double) (t * cos(t));
      pts[i][1]=0.0;
      pts[i][2]=(double) (t * sin(t));

      // base coloring on position
      int index=(int) ((float) i / (float) SIZE * 255.0);
      const float* color=colorMap.getColor(255 - index);

      clr[i][0]=color[0];
      clr[i][1]=color[1];
      clr[i][2]=color[2];
      clr[i][3]=1.0;
   }

   for (unsigned int i=0; i < SIZE; i++)
   {
      pts[i][0]/=total_angle;
      pts[i][2]/=total_angle;
   }

   // create new display list
   glNewList(dataItem->iconDisplayListId, GL_COMPILE);

   // save current attribute state
   glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
   glDisable(GL_LIGHTING);

   // draw line as generalized cylinder
   glePolyCylinder_c4f(SIZE, // num points in polyline
   pts, // polyline vertces
   clr, // colors at polyline verts
   0.05); // radius of cylinder

   // restore previous attribute state
   glPopAttrib();

   glEndList();
}

void StaticSolverTool::render(DTS::DataItem* dataItem) const
{
   if(dataItem->dataDisplayListVersion != dataDisplayListVersion)
   {
      updateDataDisplayList(dataItem);
      /* Mark the display list as up-to-date: */
      dataItem->dataDisplayListVersion = dataDisplayListVersion;
   } 
   glCallList(dataItem->dataDisplayListId);
}

void StaticSolverTool::setIntegrator(Integrator* i)
{
   integrator=i;

   if (not datasets.empty())
   {
      // Remove all StaticSolverData instances except the last one
      StaticSolverData* lastData = datasets.back();
      computeStaticSolution(lastData);
      std::vector<StaticSolverData*>::iterator it;
      for (it = datasets.begin(); it != datasets.end()-1; it++)
      {
         delete *it;
      }
      datasets.clear();
      datasets.push_back(lastData);
   }
   requestDataDisplayListUpdate();
}

void StaticSolverTool::updatedIntegrator()
{
   // recompute all static solutions
   std::vector<StaticSolverData*>::iterator it;
   for (it = datasets.begin(); it != datasets.end(); it++)
   {
      computeStaticSolution(*it);
   }
   requestDataDisplayListUpdate();
}


void StaticSolverTool::step()
{
}

void StaticSolverTool::moved(const ToolBox::MotionEvent & motionEvent)
{
}

void StaticSolverTool::mainButtonPressed(const ToolBox::ButtonPressEvent & buttonReleaseEvent)
{
}

void StaticSolverTool::mainButtonReleased(const ToolBox::ButtonReleaseEvent & buttonReleaseEvent)
{
   // get the current locator position
   pos=toolBox()->deviceTransformationInModel().getOrigin();

   std::cout << "Position: (" << pos[0] << ", " << pos[1] << ", " << pos[2] << ")" << std::endl;
   // create a new static solution
   StaticSolverData* newData = new StaticSolverData;
   newData->points[0]=pos;
   computeStaticSolution(newData);

   if (not multipleStaticSolutions)
   {
      clearDatasets();
   }

   datasets.push_back(newData);
   requestDataDisplayListUpdate();
}

//
// StaticSolverTool internal methods
//

void StaticSolverTool::drawBasicLine(StaticSolverData* d) const
{
   // save the current attribute state
   glPushAttrib(GL_LIGHTING_BIT);

   glDisable(GL_LIGHTING);

   if (datasets[0]->colorStyle == StaticSolverData::SOLID)
   {
      glColor3f(1.0f, 0.5f, 0.0f);

      glBegin(GL_LINE_STRIP);
      for (unsigned int i=0; i < datasets[0]->numberOfPoints; i++)
      {
         glVertex3f(d->points[i][0], d->points[i][1], d->points[i][2]);
      }
      glEnd();

   }
   else if (datasets[0]->colorStyle == StaticSolverData::GRADIENT)
   {
      glBegin(GL_LINES);
      for (unsigned int i=1; i < datasets[0]->numberOfPoints; i++)
      {
         const unsigned int index=(int) ((float) i / (float) datasets[0]->numberOfPoints
               * 255.0);

         const float* color=datasets[0]->colorMap->getColor(index);
         glColor3fv(color);

         glVertex3f(d->points[i - 1][0], d->points[i - 1][1], d->points[i
               - 1][2]);
         glVertex3f(d->points[i][0], d->points[i][1], d->points[i][2]);
      }
      glEnd();

   }

   // restore the previous attribute state
   glPopAttrib();
}

void StaticSolverTool::drawPolyLine(StaticSolverData* d) const
{
   // save the current attribute state
   glPushAttrib(GL_LIGHTING_BIT);

   // allocate memory for gle rendering methods
   gleDouble points[datasets[0]->numberOfPoints][3];
   float colors[datasets[0]->numberOfPoints][3];
   gleDouble radius=0.1; // radius of poly-cylinder

   if (datasets[0]->colorStyle == StaticSolverData::SOLID)
   {
      glEnable(GL_LIGHTING);

      GLMaterial
            material(GLMaterial::Color(1.0, 0.5, 0.0, 1.0), GLMaterial::Color(1.0, 1.0, 1.0, 1.0), 80.0);

      glMaterial(GLMaterialEnums::FRONT_AND_BACK, material);

      for (unsigned int i=0; i < datasets[0]->numberOfPoints; i++)
      {
         points[i][0]=d->points[i][0];
         points[i][1]=d->points[i][1];
         points[i][2]=d->points[i][2];
      }
   }
   else if (datasets[0]->colorStyle == StaticSolverData::GRADIENT)
   {
      glDisable(GL_LIGHTING);

      for (unsigned int i=0; i < datasets[0]->numberOfPoints; i++)
      {
         unsigned int index=(int) ((float) i / (float) datasets[0]->numberOfPoints
               * 255.0);

         const float* color=datasets[0]->colorMap->getColor(index);
         glColor3fv(color);

         colors[i][0]=color[0];
         colors[i][1]=color[1];
         colors[i][2]=color[2];

         points[i][0]=d->points[i][0];
         points[i][1]=d->points[i][1];
         points[i][2]=d->points[i][2];
      }
   }

   // draw line as generalized cylinder
   glePolyCylinder(datasets[0]->numberOfPoints, // num points in polyline
   points, // polyline vertces
   colors, // colors at polyline verts
   radius); // radius of polycylinder

   // restore previous attribute state
   glPopAttrib();
}

/* Private methods */

void StaticSolverTool::computeStaticSolution(StaticSolverData* data)
{
   for (unsigned int i=1; i < data->numberOfPoints; i++)
   {  
      data->points[i] = data->points[i-1];
      Geometry::Point<double, 3> tmp(data->points[i-1][0],
                                     data->points[i-1][1],
                                     data->points[i-1][2]);
      data->points[i] += integrator->step(tmp);
   }   

}


void StaticSolverTool::clearDatasets()
{
   // clear all dynamically allocated StaticSolverData instances
   std::vector<StaticSolverData*>::iterator it;
   for (it = datasets.begin(); it != datasets.end(); it++)
   {
      delete *it;
   }
   datasets.clear();
}

/** When StaticSolverTool::dataDisplayListVersion is different from
 *  DTS::DataItem::dataDisplayListVersion, the display lists are updated
 *  at the next opportunity (in StaticSolverTool::render method).
 */
void StaticSolverTool::requestDataDisplayListUpdate()
{
   ++dataDisplayListVersion;
}

void StaticSolverTool::updateDataDisplayList(DTS::DataItem* dataItem) const
{
   // create new display list
   glNewList(dataItem->dataDisplayListId, GL_COMPILE);

   std::vector<StaticSolverData*>::const_iterator it;
   // Recall: 'it' is a pointer to a pointer of a StaticSolverData instance.
   for (it = datasets.begin(); it != datasets.end(); it++)
   {
      // delegate rendering based on line style
      if (datasets[0]->lineStyle == StaticSolverData::BASIC)
      {
         drawBasicLine(*it);
      }
      else if (datasets[0]->lineStyle == StaticSolverData::POLY_LINE)
      {
         drawPolyLine(*it);
      }
   }

   glEndList();
}

