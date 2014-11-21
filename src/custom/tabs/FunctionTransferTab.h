//
//  FunctionTransferTab.h
//  ShapeAnalyzer
//
//  Created by Emanuel Laude on 24.08.14.
//
//

#ifndef __ShapeAnalyzer__FunctionTransferTab__
#define __ShapeAnalyzer__FunctionTransferTab__

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>

#include <QMessageBox>

#include "../../domain/Shape.h"
#include "../../domain/correspondences/PointCorrespondenceData.h"
#include "../../domain/attributes/ScalarPointAttribute.h"
#include "../../domain/metric/GeodesicMetric.h"
#include "../../domain/laplaceBeltrami/FEMLaplaceBeltramiOperator.h"
#include "../../domain/signatures/WaveKernelSignature.h"
#include "../../domain/FunctionalMaps.h"
#include "../../util/HashMap.h"

#include "../../view/CustomListWidgetItem.h"

#include "CustomTab.h"

#include "ui_FunctionTransferTabWidget.h"

class FunctionTransferTab : public QWidget, private Ui::FunctionTransferTabWidget, public CustomTab {
    Q_OBJECT
    
public:
    FunctionTransferTab(const HashMap<vtkActor*, Shape*>& shapes, const HashMap<PointCorrespondenceData*, bool>& pointCorrespondences, const HashMap<FaceCorrespondenceData*, bool>& faceCorrespondences, QWidget* parent);
    
    virtual ~FunctionTransferTab();
    
    virtual void onShapeDelete(Shape* shape);
    virtual void onShapeAdd(Shape* shape);
    virtual void onShapeEdit(Shape* shape);
    virtual void onClear();
private slots:
    virtual void slotTransfer();
};

#endif /* defined(__ShapeAnalyzer__FunctionTransferTab__) */