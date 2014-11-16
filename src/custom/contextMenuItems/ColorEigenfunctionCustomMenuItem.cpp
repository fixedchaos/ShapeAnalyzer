//
//  ColorEigenfunctionCustomMenuItem.cpp
//  ShapeAnalyzer
//
//  Created by Emanuel Laude on 12.11.14.
//
//

#include "ColorEigenfunctionCustomMenuItem.h"


void ColorEigenfunctionCustomMenuItem::onClick(Shape* shape, vtkIdType pointId, vtkIdType faceId, QWidget* parent) {
    bool ok;
    int i = QInputDialog::getInt(
                                 parent,
                                 "Eigenfunction",
                                 "Choose an eigenfunction.",
                                 0,
                                 0,
                                 std::min((vtkIdType) 99, shape->getPolyData()->GetNumberOfPoints()),
                                 1,
                                 &ok
                                 );
    // show eigenfunction
    if (ok) {
        ScalarPointAttribute eigenfunction(shape);
        
        LaplaceBeltramiOperator* laplacian = Factory<LaplaceBeltramiOperator>::getInstance()->create("fem");
        laplacian->initialize(shape, 100);
        laplacian->getEigenfunction(i, eigenfunction);
        delete laplacian;
        
        shape->colorPointsScalars(eigenfunction.getScalars());
    }
}
