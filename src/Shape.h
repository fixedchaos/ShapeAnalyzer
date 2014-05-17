#ifndef Shape_H
#define Shape_H

#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkIdList.h>
#include <vtkLinearTransform.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkRenderWindow.h>
#include <vtkCell.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkLinearTransform.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkTriangle.h>
#include <vtkGlyph3D.h>
#include <vtkSphereSource.h>

#include <math.h>
#include <iostream>
#include <string>
#include <sstream>

#include "vtkGeodesic.h"


using namespace std;

class Shape {
public:
    Shape(vtkIdType shapeID, vtkSmartPointer<vtkPolyData> polyData, vtkSmartPointer<vtkRenderer> renderer);

    Shape(vtkSmartPointer<vtkRenderer> renderer);
    
    double      getEuclideanDistances(int start, std::vector<double> &result);
    void        visualizeEuclidean(int start = -1);
    
    vtkSmartPointer<vtkIdList>  getVoronoiCells(vtkSmartPointer<vtkIdList> points);
    void                        visualizeVoronoiCells();
    void                        visualizeVoronoiCells(vtkSmartPointer<vtkIdList> points);
    
    vtkSmartPointer<vtkIdList>  getFPS(unsigned numberSamples, int start = -1);
    void                        setFPS(unsigned numberSamples, int start = -1);
    void                        transformFPS(vtkLinearTransform* t);
    
    // getters
    vtkSmartPointer<vtkActor> getActor() {
        return actor_;
    }
    
    vtkSmartPointer<vtkBoxWidget> getBoxWidget() {
        return boxWidget_;
    }
    
    vtkSmartPointer<vtkIdList> getCurrentFPS() {
        return fps_;
    }
    
    vtkSmartPointer<vtkActor> getFPSActor() {
        return fpsActor_;
    }
    
    vtkSmartPointer<vtkPolyData> getPolyData() {
        return polyData_;
    }

    vtkSmartPointer<vtkPolyDataNormals> getPolyDataNormals() {
        return polyDataNormals_;
    }
    
    vtkSmartPointer<vtkPolyDataMapper> getMapper() {
        return mapper_;
    }
    
    vtkIdType getId() {
        return shapeId_;
    }
    
    void remove();
    
    ostream& write(ostream& os);
    
    friend ostream& operator<<(ostream& os, const Shape& shape);
    
    istream& read(istream& is);
    
    friend istream& operator>>(istream& is, Shape& shape);
private:
    void initialize();
    
    
    vtkIdType shapeId_;
    
    vtkSmartPointer<vtkActor> actor_;
    vtkSmartPointer<vtkPolyDataMapper> mapper_;
    vtkSmartPointer<vtkBoxWidget> boxWidget_;
    vtkSmartPointer<vtkPolyData> polyData_;
    vtkSmartPointer<vtkPolyDataNormals> polyDataNormals_;
    vtkSmartPointer<vtkRenderer>        renderer_;
    
    vtkSmartPointer<vtkIdList>          fps_;
    vtkSmartPointer<vtkActor>           fpsActor_;
    
    vtkSmartPointer<vtkIdList>          voronoiCells_;
    vtkSmartPointer<vtkActor>           voronoiCellsActor_;

};

#endif