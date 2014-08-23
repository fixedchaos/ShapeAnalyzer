//
//  GeodesicMetric.h
//  ShapeAnalyzer
//
//  Created by Zorah on 27.05.14.
//
//

#ifndef __ShapeAnalyzer__GeodesicMetric__
#define __ShapeAnalyzer__GeodesicMetric__

#include <iostream>

#include "Metric.h"
#include "../Shape.h"

#include <vtkCellData.h>
#include <vtkIdList.h>
#include <vtkLookupTable.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>

#include "3rdparty/geodesic/geodesic_algorithm_exact.h"
#include "3rdparty/geodesic/geodesic_error.h"

using namespace geodesic;
using namespace std;

class GeodesicMetric : public Metric {

///////////////////////////////////////////////////////////////////////////////
// Declaration
///////////////////////////////////////////////////////////////////////////////
private:
    ///////////////////////////////////////////////////////////////////////////////
    // Nested Classes
    ///////////////////////////////////////////////////////////////////////////////
    
    // for using vtkPolyData in the geodesic algorithm
    class geodesicPoints {
    public:
        geodesicPoints(vtkSmartPointer<vtkPolyData> polyData) : polyData_(polyData) {}
        
        vtkIdType size() {
            return polyData_->GetNumberOfPoints() * 3;
        }
        
        double operator[](vtkIdType i) {
            vtkIdType pointId = i / 3;
            vtkIdType component = i % 3;
            double point[3];
            polyData_->GetPoint(pointId, point);
            return point[component];
        }
        
        vtkSmartPointer<vtkPolyData> polyData_;
    };
    
    // for using vtkPolyData in the geodesic algorithm
    class geodesicFaces {
    public:
        geodesicFaces(vtkSmartPointer<vtkPolyData> polyData) : polyData_(polyData) {}
        
        vtkIdType size() {
            return polyData_->GetNumberOfCells() * 3;
        }
        
        vtkIdType operator[](vtkIdType i) {
            vtkIdType cellId = i / 3;
            vtkIdType pointId = i % 3;
            
            vtkSmartPointer<vtkIdList> ids = vtkSmartPointer<vtkIdList>::New();
            polyData_->GetCellPoints(cellId, ids);
            return ids->GetId(pointId);
        }
        
        vtkSmartPointer<vtkPolyData> polyData_;
    };
    
    GeodesicMetric() : algorithm_(nullptr), points_(nullptr), faces_(nullptr) {
        
    }
    
    double  calculateLengthOfPath(vector<SurfacePoint> path);
    
    Mesh                                mesh_;
    GeodesicAlgorithmExact*             algorithm_;
    geodesicPoints*                     points_;
    geodesicFaces*                      faces_;
    vector<SurfacePoint>                sources_;
    vtkSmartPointer<vtkIdList>          sourceList_;
  
    

    
    
public:
    static Metric* create() {
        return new GeodesicMetric();
    }
    
    static string getIdentifier() {
        return "geodesic";
    }
    
    virtual void initialize(Shape* shape);
    
    virtual ~GeodesicMetric();
    
    // from abstract class Metric
    virtual double getDistance(vtkIdType a, vtkIdType b);
    virtual void getAllDistances(ScalarPointAttribute& distances, vtkIdType source);
    virtual vtkIdType getPointFarthestFromAllSources(vtkSmartPointer<vtkIdList> sources);
    
    // Geodesic functions
    void        changeSourcePoint(vtkIdType source);
    void        changeSourcePoints(vtkSmartPointer<vtkIdList> sources);
    
    vtkIdType   getPointFarthestFromAllSources();

    
};

#endif /* defined(__ShapeAnalyzer__GeodesicMetric__) */