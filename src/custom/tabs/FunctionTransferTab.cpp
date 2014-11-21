
//
//  FunctionTransferTab.cpp
//  ShapeAnalyzer
//
//  Created by Emanuel Laude on 24.08.14.
//
//

#include "FunctionTransferTab.h"

FunctionTransferTab::~FunctionTransferTab() {
}

FunctionTransferTab::FunctionTransferTab(const HashMap<vtkActor*, Shape*>& shapes, const HashMap<PointCorrespondenceData*, bool>& pointCorrespondences, const HashMap<FaceCorrespondenceData*, bool>& faceCorrespondences, QWidget* parent) : QWidget(parent, 0), CustomTab(shapes, pointCorrespondences, faceCorrespondences, parent) {
    this->setupUi(this);
    
    QStringList labels;
    for(auto entry : shapes_) {

        QString label = QString::number(entry.second->getId());
        label.append(QString::fromStdString(":"+entry.second->getName()));
        labels << label;

    }
    
    if(shapes_.size() < 2) {
        buttonTransfer->setEnabled(false);
    }
    
    comboBoxSourceShape->insertItems(0, labels);
    comboBoxTargetShape->insertItems(0, labels);
    
    connect(this->buttonTransfer,                   SIGNAL(released()),
            this,                                   SLOT(slotTransfer()));
}

void FunctionTransferTab::slotTransfer() {
    vtkIdType sid = comboBoxSourceShape->currentText().split(':')[0].toInt();
    vtkIdType tid = comboBoxTargetShape->currentText().split(':')[0].toInt();

    
    Shape* source = nullptr;
    Shape* target = nullptr;
    for(auto entry : shapes_) {
        if(sid == entry.second->getId()) {
            source = entry.second;
        }
        
        if(tid == entry.second->getId()) {
            target = entry.second;
        }
    }
    if(source == target) {
        QMessageBox::warning(parent_, "Error", "The shapes source \"" + QString(source->getName().c_str()) + "\" and target \"" + QString(source->getName().c_str()) + "\" have to be diffrent.");
        return;
    }
    

    
    if(source->getColoring() == nullptr || (source->getColoring()->type != Shape::Coloring::Type::PointScalar && source->getColoring()->type != Shape::Coloring::Type::PointSegmentation)) {
        QMessageBox::warning(parent_, "Error", "Shape \"" + QString(source->getName().c_str()) + "\" does neither have a scalar point map nor a segmentation. Compute scalar point map or a segmentation first.");
        return;
    }
    attribute::ScalarPointAttribute f(source);
    for(vtkIdType i = 0; i < source->getColoring()->values->GetNumberOfTuples(); i++) {
        f.getScalars()->SetValue(i, source->getColoring()->values->GetTuple(i)[0]);
    }
    
    // initialize lists of corresponding contraints on both shapes. Ordering represents correspondence of contraints. I.e. c1[5] on shape1 corresponds to c2[5] on shape2.
    vector<attribute::ScalarPointAttribute> cs; // corresponds to contraints on shape1
    vector<attribute::ScalarPointAttribute> ct;
    
    
    // compute landmark matches using all available correspondences between shape1 and shape2 and geodesic metric
    metric::GeodesicMetric ms(source);
    metric::GeodesicMetric mt(target);

    
    for(auto entry : pointCorrespondences_) {
        PointCorrespondenceData* corr = entry.first;
        
        for(int i = 0; i < corr->getShapeIds().size(); i++) {
            if(corr->getShapeIds()[i] == source->getId()) {
                
                
                ScalarPointAttribute distances(source);
                ms.getAllDistances(distances, corr->getCorrespondingIds()[i]);
                cs.push_back(distances);
                
            }
            
            if(corr->getShapeIds()[i] == target->getId()) {
                
                ScalarPointAttribute distances(target);
                mt.getAllDistances(distances, corr->getCorrespondingIds()[i]);
                ct.push_back(distances);
                
            }
        }
    }
    
    laplaceBeltrami::FEMLaplaceBeltramiOperator laplacianSource(source, 100);
    laplaceBeltrami::FEMLaplaceBeltramiOperator laplacianTarget(target, 100);
    
    
    // compute 200-dimensional wave kernel discriptor on both shapes
    WaveKernelSignature wksSource(source, 200, &laplacianSource);
    
    
    WaveKernelSignature wksTarget(target, 200, &laplacianTarget);
    
    // use first 125 components of wave kernel signature as additional constraints. Truncate rest because wave kernel seems to be inaccurate in higher dimensions
    for(int i = 0; i < 200; i++) {
        ScalarPointAttribute wksiSource(source);
        wksSource.getComponent(i, wksiSource);
        cs.push_back(wksiSource);
        
        ScalarPointAttribute wksiTarget(target);
        wksTarget.getComponent(i, wksiTarget);
        ct.push_back(wksiTarget);
    }
    
    // compute correspondence matrix C
    FunctionalMaps functionalMaps(*source, *target, &laplacianSource, &laplacianTarget, cs, ct, 100);

    
    // transfer the coordinate function
    ScalarPointAttribute Tf(target);
    functionalMaps.transferFunction(f, Tf);
    
    // color 2nd shape
    shared_ptr<Shape::Coloring> coloring = make_shared<Shape::Coloring>();
    coloring->type = Shape::Coloring::Type::PointScalar;
    coloring->values = Tf.getScalars();
    target->setColoring(coloring);
}


void FunctionTransferTab::onShapeAdd(Shape* shape) {
    QString label = QString::number(shape->getId());
    label.append(QString::fromStdString(":"+shape->getName()));
    comboBoxSourceShape->insertItem(0, label);
    comboBoxTargetShape->insertItem(0, label);
    this->buttonTransfer->setEnabled(true);
}

void FunctionTransferTab::onShapeDelete(Shape* shape) {
    for(int i = comboBoxSourceShape->count()-1; i >= 0; i--) {
        if(comboBoxSourceShape->itemText(i).split(':')[0].toInt() == shape->getId()) {
            comboBoxSourceShape->removeItem(i);
            break;
        }
    }
    
    for(int i = comboBoxTargetShape->count()-1; i >= 0; i--) {
        if(comboBoxTargetShape->itemText(i).split(':')[0].toInt() == shape->getId()) {
            comboBoxTargetShape->removeItem(i);
            break;
        }
    }
    if(comboBoxSourceShape->count() == 0) {
        this->buttonTransfer->setEnabled(false);
    }
}

void FunctionTransferTab::onShapeEdit(Shape* shape) {
    QString label = QString::number(shape->getId());
    label.append(QString::fromStdString(":"+shape->getName()));
    
    for(int i = comboBoxSourceShape->count()-1; i >= 0; i--) {
        if(comboBoxSourceShape->itemText(i).split(':')[0].toInt() == shape->getId()) {
            comboBoxSourceShape->setItemText(i, label);
            break;
        }
    }
    
    for(int i = comboBoxTargetShape->count()-1; i >= 0; i--) {
        if(comboBoxTargetShape->itemText(i).split(':')[0].toInt() == shape->getId()) {
            comboBoxTargetShape->setItemText(i, label);
            break;
        }
    }
}

void FunctionTransferTab::onClear() {
    this->comboBoxSourceShape->clear();
    this->comboBoxTargetShape->clear();
    this->buttonTransfer->setEnabled(false);
}
