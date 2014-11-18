//
//  Metric.h
//  ShapeAnalyzer
//
//  Created by Zorah on 27.05.14.
//
//

#ifndef __ShapeAnalyzer__Metric__
#define __ShapeAnalyzer__Metric__

#include <iostream>

#include "../Shape.h"
#include "../attributes/ScalarPointAttribute.h"

#include <vtkIdList.h>
#include <vtkSmartPointer.h>

using namespace std;
using namespace attribute;

namespace metric {

///
/// \brief Abstract class for the representation of a metric on the vertices of a shape.
/// \details Besides a pure pairwise distance function getDistance(vtkIdType, vtkIdType) this class provides functionality to obtion distance vectors getAllDistances(ScalarPointAttribute&, vtkIdType) containing the distances from one source vertex to all other vertices or functions to create voronoi cells and farthest point samplings.
/// \author Emanuel Laude and Zorah Lähner
///

class Metric {
 
    
public:
    /// Protected constructor since Metric should only be initialized via Factory
    Metric(Shape* shape) : shape_(shape) {}
    
    /// Empty destructor
    virtual ~Metric() {};

    /// Returns the distance between the points with the ids a and b
    virtual double getDistance(vtkIdType a, vtkIdType b) = 0;
    
    /// Returns the distances from vertex with id source to all other vertices. The result is stored in distances which is of type ScalarPointAttribute.
    virtual void getAllDistances(ScalarPointAttribute& distances, vtkIdType source) = 0;
    
    /// Returns the id of the point with the greatest distance to all points in the source list
    virtual vtkIdType getFarthestPoint(vtkSmartPointer<vtkIdList> sources) = 0;
    
    /// Returns voronoi cells that correspond to the vertices in argument seeds. The result is of type vtkIntArray and it contains the corresponding cell id for each vertex.
    virtual vtkSmartPointer<vtkIntArray> getVoronoiCells(vtkSmartPointer<vtkIdList> seeds) = 0;
protected:
    Shape* shape_;
};
    
}

#endif /* defined(__ShapeAnalyzer__Metric__) */
