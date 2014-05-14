//
//  PointCorrespondence.h
//  ShapeAnalyzer
//
//  Created by Emanuel Laude on 08.05.14.
//
//

#ifndef PointCorrespondence_H
#define PointCorrespondence_H

#include "Correspondence.h"
#include "PointCorrespondeceData.h"

class PointCorrespondence : public Correspondence {
public:
    PointCorrespondence(vtkSmartPointer<vtkRenderer> renderer, Shape* shape1, Shape* shape2, vtkIdType point1Id, vtkIdType point2Id, vtkSmartPointer<vtkActor> actor1, vtkSmartPointer<vtkActor> actor2);
    
};

#endif /* defined(PointCorrespondence_H) */
