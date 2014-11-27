//
//  HeatDiffusion.h
//  ShapeAnalyzer
//
//  Created by Emanuel Laude on 30.07.14.
//
//

#ifndef __ShapeAnalyzer__HeatDiffusion__
#define __ShapeAnalyzer__HeatDiffusion__


#include <vtkPolyData.h>

#include "Shape.h"

using namespace laplaceBeltrami;

class HeatDiffusion {
public:
    HeatDiffusion(Shape* shape, vtkSmartPointer<vtkDoubleArray> initialCondition) : shape_(shape), initialCondition_(initialCondition) {}
    ~HeatDiffusion();
    virtual vtkSmartPointer<vtkDoubleArray> getHeat(double t) = 0;
private:
    Shape* shape_;
    vtkSmartPointer<vtkDoubleArray> initialCondition_;
};

#endif /* defined(__ShapeAnalyzer__HeatDiffusion__) */
