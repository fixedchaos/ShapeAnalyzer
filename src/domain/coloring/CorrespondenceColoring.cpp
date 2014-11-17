//
//  CorrespondenceColoring.cpp
//  ShapeAnalyzer
//
//  Created by Zorah on 08.07.14.
//
//

#include "CorrespondenceColoring.h"
#include <utility>

///////////////////////////////////////////////////////////////////////////////
coloring::CorrespondenceColoring::CorrespondenceColoring(HashMap<vtkActor*, Shape*> const*               set,
                                               HashMap<PointCorrespondenceData*, bool> const*  points,
                                               HashMap<FaceCorrespondenceData*, bool> const*  faces,
                                               Shape* reference)
: shapes_(set), points_(points), faces_(faces), reference_(reference)
{
    if(reference_ == 0) {
        reference_ = shapes_->begin()->second;
    }
}


///////////////////////////////////////////////////////////////////////////////
void coloring::CorrespondenceColoring::showPointCorrespondences(vector<pair<vtkIdType, double> >* percentageMatched,
                                                      vector<pair<vtkIdType, double> >* percentageMultiple) {
    pointAttributes_.clear();
    
    // completely color reference shape
    CoordinateColoring cc = CoordinateColoring(reference_);
    cc.color();
    
    vtkIdType referenceId = reference_->getId();
    vtkSmartPointer<vtkUnsignedCharArray> referenceColors = cc.getColors();
    pointAttributes_.insert(
                        make_pair(referenceId, referenceColors));
    
    // for evaluating matched points
    unordered_map<vtkIdType, vector<int>* > matched;
    
    // initialize color arrays for all shapes
    for (auto it = shapes_->begin(); it != shapes_->end(); it++) {
        if(it->second != reference_) {
            vtkSmartPointer<vtkUnsignedCharArray> array = vtkSmartPointer<vtkUnsignedCharArray>::New();
            array->SetNumberOfComponents(3);
            array->SetNumberOfTuples(it->second->getPolyData()->GetPoints()->GetNumberOfPoints());
            array->SetName("Colors");
            pointAttributes_.insert(make_pair(it->second->getId(), array));
            
            // TODO sooo unschön, aber ich weiß nicht wie ich ihm vermittele, dass standardmäßig auf weiß gemappt werden soll
            double white[3] = {255, 255, 255};
            for(vtkIdType i = 0; i < it->second->getPolyData()->GetPoints()->GetNumberOfPoints(); i++) {
                array->SetTuple(i, white);
            }
            
            // if requested, evaluate matched points
            if (percentageMatched != 0 || percentageMultiple != 0) {
                vector<int>* mArray = new vector<int>(it->second->getPolyData()->GetPoints()->GetNumberOfPoints(), 0);
                matched.insert(make_pair(it->second->getId(), mArray));
            }
        }
    }
    
    // look up colors for non-reference shapes
    for(auto it = points_->begin(); it != points_->end(); it++) {
        vector<vtkIdType> shapes = it->first->getShapeIds();
        vector<vtkIdType> points = it->first->getCorrespondingIds();
        
        // id of correspondence on reference shape
        vtkIdType referenceCorrespondence = -1;
        
        // check if reference is in correspondence
        for(int i = 0; i < shapes.size(); i++) {
            if (shapes[i] == referenceId) {
                referenceCorrespondence = points[i];
                break;
            }
        }
        
        // if reference found
        if(referenceCorrespondence != -1) {
            // look up color for each shape in correspondence
            for (int i = 0; i < shapes.size(); i++) {
                // dont color reference shape
                if (shapes[i] != referenceId) {
                if(pointAttributes_.count(shapes[i]) != 0) {
                    vtkSmartPointer<vtkUnsignedCharArray> colors = pointAttributes_.find(shapes[i])->second;
                    // color of point is the same as the corresponding point on the reference shape has
                    double color[3];
                    referenceColors->GetTuple(referenceCorrespondence, color);
                    colors->SetTuple(points[i], color);
                    
                    // if requested, evaluate matched points
                    if (percentageMatched != 0 || percentageMultiple != 0) {
                        vector<int>* match = matched.find(shapes[i])->second;
                        (*match)[points[i]] = (*match)[points[i]] + 1;
                    }
                }
                }
            }
        }
    }
    
    // color all shapes
    for (auto it = shapes_->begin(); it != shapes_->end(); it++) {
        if (it->second != reference_) {
            vtkSmartPointer<vtkUnsignedCharArray> colors = pointAttributes_.find(it->second->getId())->second;
            it->second->getPolyData()->GetPointData()->SetScalars(colors);
            it->second->getMapper()->SetScalarModeToUsePointData();
            it->second->getMapper()->SetColorModeToDefault();
            it->second->getMapper()->SetScalarRange(0, 1);
            it->second->getMapper()->ScalarVisibilityOn();
            
            // if requested, evaluate matched points
            if (percentageMatched != 0 || percentageMultiple != 0) {
                vector<int>* match = matched.find(it->second->getId())->second;
                int numMatched = 0;
                int numMultiple = 0;
                
                // count matched / multiple matched points
                for (int i = 0; i < match->size(); i++) {
                    if ((*match)[i] > 0)
                        numMatched++;
                    if ((*match)[i] > 1)
                        numMultiple++;
                }
                
                // add percentage matched points
                if (percentageMatched != 0) {
                    double per = ((double) numMatched) * 100 / match->size();
                    percentageMatched->push_back(make_pair(it->second->getId(), per));
                }
                
                // add percentage mulitple matched points
                if (percentageMultiple != 0) {
                    double per = ((double) numMultiple) * 100 / match->size();
                    percentageMultiple->push_back(make_pair(it->second->getId(), per));
                }
            }
            
        }
    }
    
    for (auto it = matched.begin(); it != matched.end(); it++) {
        delete it->second;
    }
    
    // rerender
    reference_->getRenderer()->GetRenderWindow()->Render();
}

///////////////////////////////////////////////////////////////////////////////
void coloring::CorrespondenceColoring::showFaceCorrespondences(vector<pair<vtkIdType, double> >* percentageMatched,
                                                     vector<pair<vtkIdType, double> >* percentageMultiple) {
    faceAttributes_.clear();
    
    // completely color reference shape
    FaceCoordinateColoring cc = FaceCoordinateColoring(reference_);
    cc.color();
    
    vtkIdType referenceId = reference_->getId();
    vtkSmartPointer<vtkUnsignedCharArray> referenceColors = cc.getColors();
    faceAttributes_.insert(
                            make_pair(referenceId, referenceColors));
    
    // for evaluating matched points
    unordered_map<vtkIdType, vector<int>* > matched;
    
    // initialize color arrays for all shapes
    for (auto it = shapes_->begin(); it != shapes_->end(); it++) {
        if(it->second != reference_) {
            vtkSmartPointer<vtkUnsignedCharArray> array = vtkSmartPointer<vtkUnsignedCharArray>::New();
            array->SetNumberOfComponents(3);
            array->SetNumberOfTuples(it->second->getPolyData()->GetNumberOfCells());
            array->SetName("Colors");
            faceAttributes_.insert(make_pair(it->second->getId(), array));
            
            // TODO sooo unschön, aber ich weiß nicht wie ich ihm vermittele, dass standardmäßig auf weiß gemappt werden soll
            double white[3] = {255, 255, 255};
            for(vtkIdType i = 0; i < it->second->getPolyData()->GetNumberOfCells(); i++) {
                array->SetTuple(i, white);
            }
            
            // if requested, evaluate matched points
            if (percentageMatched != 0 || percentageMultiple != 0) {
                vector<int>* mArray = new vector<int>(it->second->getPolyData()->GetNumberOfCells(), 0);
                matched.insert(make_pair(it->second->getId(), mArray));
            }
        }
    }
    
    // look up colors for non-reference shapes
    for(auto it = faces_->begin(); it != faces_->end(); it++) {
        vector<vtkIdType> shapes = it->first->getShapeIds();
        vector<vtkIdType> points = it->first->getCorrespondingIds();
        
        // id of correspondence on reference shape
        vtkIdType referenceCorrespondence = -1;
        
        // check if reference is in correspondence
        for(int i = 0; i < shapes.size(); i++) {
            if (shapes[i] == referenceId) {
                referenceCorrespondence = points[i];
                break;
            }
        }
        
        if(referenceCorrespondence != -1) {
            // look up color for each shape in correspondence
            for (int i = 0; i < shapes.size(); i++) {
                // dont color reference shape
                if (shapes[i] != referenceId) {
                    if(faceAttributes_.count(shapes[i]) != 0) {
                        vtkSmartPointer<vtkUnsignedCharArray> colors = faceAttributes_.find(shapes[i])->second;
                        // color of point is the same as the corresponding point on the reference shape has
                        double color[3];
                        referenceColors->GetTuple(referenceCorrespondence, color);
                        colors->SetTuple(points[i], color);
                        
                        // if requested, evaluate matched points
                        if (percentageMatched != 0 || percentageMultiple != 0) {
                            vector<int>* match = matched.find(shapes[i])->second;
                            (*match)[points[i]] = (*match)[points[i]] + 1;
                        }
                    }
                }
            }
        }
    }
    
    // color all shapes
    for (auto it = shapes_->begin(); it != shapes_->end(); it++) {
        if(it->second != reference_) {
            vtkSmartPointer<vtkUnsignedCharArray> colors = faceAttributes_.find(it->second->getId())->second;
            it->second->getPolyData()->GetCellData()->SetScalars(colors);
            it->second->getMapper()->SetScalarModeToUseCellData();
            it->second->getMapper()->SetColorModeToDefault();
            it->second->getMapper()->SetScalarRange(0, 1);
            it->second->getMapper()->ScalarVisibilityOn();
            
            // if requested, evaluate matched points
            if (percentageMatched != 0 || percentageMultiple != 0) {
                vector<int>* match = matched.find(it->second->getId())->second;
                int numMatched = 0;
                int numMultiple = 0;
                
                // count matched / multiple matched points
                for (int i = 0; i < match->size(); i++) {
                    if ((*match)[i] > 0)
                        numMatched++;
                    if ((*match)[i] > 1)
                        numMultiple++;
                }
                
                // add percentage matched points
                if (percentageMatched != 0) {
                    double per = ((double) numMatched) * 100 / match->size();
                    percentageMatched->push_back(make_pair(it->second->getId(), per));
                }
                
                // add percentage mulitple matched points
                if (percentageMultiple != 0) {
                    double per = ((double) numMultiple) * 100 / match->size();
                    percentageMultiple->push_back(make_pair(it->second->getId(), per));
                }
            }
        }
    }
    
    // rerender
    reference_->getRenderer()->GetRenderWindow()->Render();
}