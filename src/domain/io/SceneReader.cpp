//
//  SceneReader.cpp
//  ShapeAnalyzer
//
//  Created by Zorah on 28.07.14.
//
//

#include "SceneReader.h"

///////////////////////////////////////////////////////////////////////////////
void SceneReader::exportCorrespondences(Set<CorrespondenceData*, bool>* correspondences,
                                        Set<vtkActor*, Shape*>*         shapeIds,
                                        ofstream*                       os)
{
    // write down all correspondences
    for (auto it = correspondences->begin(); it != correspondences->end(); it++) {
        vector<vtkIdType> shapes = it->first->getShapes();
        vector<vtkIdType> data = it->first->getCorrespondingIds();
        // get the same order of shapes for all lines
        for (auto shapeIt = shapeIds->begin(); shapeIt != shapeIds->end(); shapeIt++) {
            bool contained = false;
            // check for each possible shape if it is in this correspondences
            for (int i = 0; i < shapes.size(); i++) {
                if (shapes[i] == shapeIt->second->getId()) {
                    // data[i] is the id of the correspondence element on the shape[i]
                    *os << data[i] << " ";
                    contained = true;
                }
            }
            // -1 indicates this shape is not in the correspondence
            if (!contained) {
                *os << "-1 ";
            }
        }
        // one line for each correspondence
        *os << endl;
    }
}


///////////////////////////////////////////////////////////////////////////////
void SceneReader::exportPointCorrespondences(Set<PointCorrespondenceData*, bool>*   correspondences,
                                             Set<vtkActor*, Shape*>*                shapes,
                                             string                                 filename)
{
    ofstream os(filename);
    
    // basic information, point correspondences, number of shapes
    // number of correspondences
    os << "P " << " " << shapes->size() << " " << correspondences->size() << endl;
    
    exportCorrespondences((Set<CorrespondenceData*, bool>*) correspondences, shapes, &os);
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void SceneReader::exportFaceCorrespondences(Set<FaceCorrespondenceData*, bool>* correspondences,
                                            Set<vtkActor*, Shape*>*             shapes,
                                            string                              filename)
{
    ofstream os(filename);
    
    // basic information, face correspondences, number of shapes
    // number of correspondences
    os << "F " << shapes->size() << " " << correspondences->size() << endl;
    
    exportCorrespondences((Set<CorrespondenceData*, bool>*) correspondences, shapes, &os);
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void SceneReader::loadCorrespondences(string                                filename,
                                      Set<PointCorrespondenceData*, bool>*  pointCorr,
                                      Set<FaceCorrespondenceData*, bool>*   faceCorr,
                                      QListWidget*                          shapes)
{
    ifstream is(filename);
    string line;
    
    // read 3 lines, type of correspondences, number of shapes, number of correspondences
    char type;
    unsigned int numberOfShapes, numberOfCorrespondences;
    
    stringstream ss;
    getline(is, line);
    ss << line;
    ss >> type >> numberOfShapes >> numberOfCorrespondences;
    
    cout << "NOS"<<numberOfShapes<<endl;
    cout << shapes->count()<<endl;
    
    // stop if there not enough shapes to match
    if (shapes->count() >= numberOfShapes) {
        unordered_map<string, Shape*> allShapes = createShapeMap(shapes);
        vector<Shape*> correspondingShapes = vector<Shape*>(0);
        
        // user chooses numberOfShapes many shapes to correspond
        for (int i = 0; i < numberOfShapes; i++) {
            QStringList shapeNames;
            // add all shapes that have not been chosen yet
            for (auto it = allShapes.begin(); it != allShapes.end(); it++) {
                shapeNames << QString::fromStdString(it->first);
            }
            // TODO the nullptr is not good, the windows move done...
            QString chosen = QInputDialog::getItem(nullptr,
                                                   QObject::tr("Choose a shape"),
                                                   QObject::tr("Shape"),
                                                   shapeNames);
            // add chosen shape to vector
            Shape* shape = allShapes.find(chosen.toStdString())->second;
            correspondingShapes.push_back(shape);
            // delete chosen shape from map, so it can not be chosen again
            allShapes.erase(chosen.toStdString());
        }
        
        // create correspondeneces
        for (int i = 0; i < numberOfCorrespondences; i++) {
            CorrespondenceData* data;
            if (type == 'F') {
                data = new FaceCorrespondenceData();
            } else {
                data = new PointCorrespondenceData();
            }
            
            stringstream ss;
            getline(is, line);
            ss << line;
            createData(correspondingShapes, &ss, data);
            
            if (type == 'F') {
                faceCorr->add((FaceCorrespondenceData*) data, false);
            } else {
                pointCorr->add((PointCorrespondenceData*) data, false);
            }
        }
    } else {
        QMessageBox::information(nullptr, QObject::tr("Error"), QObject::tr("There are not enough Shapes to load this Correspondences."));
    }
}

///////////////////////////////////////////////////////////////////////////////
// the items in the widget must be qtListWidgetItem<Shape*>
unordered_map<string, Shape*> SceneReader::createShapeMap(QListWidget* shapes) {
    unordered_map<string, Shape*> map;
    
    for (int i = 0; i < shapes->count(); i++) {
        qtListWidgetItem<Shape>* item = (qtListWidgetItem<Shape>*) shapes->item(i);
        Shape* shape = item->getItem();
        pair<string, Shape*> newPair = make_pair<string, Shape*>(item->text().toStdString(), shape);
        map.insert(newPair);
    }
    
    return map;
}

///////////////////////////////////////////////////////////////////////////////
void SceneReader::createData(vector<Shape*> shapes, stringstream* ss, CorrespondenceData* data) {
    for (auto it = shapes.begin(); it != shapes.end(); it++) {
        int id;
        *ss >> id;
        if (id > -1) {
            data->addData((*it)->getId(), id);
        }
    }
}