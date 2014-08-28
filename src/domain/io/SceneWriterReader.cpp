//
//  SceneWriterReader.cpp
//  ShapeAnalyzer
//
//  Created by Zorah on 28.07.14.
//
//

#include "SceneWriterReader.h"


///////////////////////////////////////////////////////////////////////////////
void SceneWriterReader::importSceneBinary(string filename, vtkSmartPointer<vtkRenderer> renderer, int& lastInsertShapeID, vector<Shape*>& shapes) {

    
    //open input file stream in binary mode
    ifstream is(filename, ios::binary);
    
    //read number of shapes
    int64_t numberOfShapes;
    is.read(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
    
    
    //read last insert shape ID
    is.read(reinterpret_cast<char*>(&lastInsertShapeID), sizeof(int64_t));
    
    //read shapes
    for(unsigned int i = 0; i < numberOfShapes; i++) {
        Shape* shape = new Shape(renderer);
        
        shape->readBinary(is);
        shapes.push_back(shape);
    }
    
    is.close();
}


///////////////////////////////////////////////////////////////////////////////
void SceneWriterReader::exportSceneBinary(string filename, vector<Shape*>& shapes, int lastInsertShapeID) {
    ofstream os(filename, ios::binary);
    int64_t numberOfShapes = (int64_t) shapes.size();
    os.write(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
    
    int64_t lastInsertID = (int64_t) lastInsertShapeID;
    os.write(reinterpret_cast<char*>(&lastInsertID), sizeof(int64_t));
    
    
    for(int i = 0; i < shapes.size(); i++) {
        shapes[i]->writeBinary(os);
    }
    
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void SceneWriterReader::importSceneASCII(string filename, vtkSmartPointer<vtkRenderer> renderer, int& lastInsertShapeID, vector<Shape*>& shapes) {
    ifstream is(filename);
    
    string line;
    
    unsigned int numberOfShapes;
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> numberOfShapes;
    }
    
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> lastInsertShapeID;
    }
    
    for(unsigned int i = 0; i < numberOfShapes; i++) {
        Shape* shape = new Shape(renderer);
        shape->readASCII(is);
        shapes.push_back(shape);
        
    }
    
    is.close();
}

///////////////////////////////////////////////////////////////////////////////
void SceneWriterReader::exportSceneASCII(string filename, vector<Shape*>& shapes, int lastInsertShapeID) {
    ofstream os(filename);
    
    os << shapes.size() << endl;
    os << lastInsertShapeID << endl;
    for(int i = 0; i < shapes.size(); i++) {
        shapes[i]->writeASCII(os);
    }
    os.close();
}



///////////////////////////////////////////////////////////////////////////////
void SceneWriterReader::exportCorrespondences(HashMap<CorrespondenceData*, bool>&   correspondences,
                                        vector<Shape*>&                             shapesOrderedById,
                                        ostream&                                    os)
{
    // write down all correspondences
    for (auto it = correspondences.begin(); it != correspondences.end(); it++) {
        vector<vtkIdType> shapeIds = it->first->getShapeIds();
        vector<vtkIdType> correspondingIds = it->first->getCorrespondingIds();
        
        // get the same order of shapes for all lines
        for (int i = 0; i < shapesOrderedById.size(); i++) {
            bool contained = false;
            // check for each possible shape whether it is in this correspondences or not
            for (int j = 0; j < shapeIds.size(); j++) {
                if (shapeIds[j] == shapesOrderedById[i]->getId()) {
                    // data[i] is the id of the correspondence element on the shape[i]
                    os << correspondingIds[j] << " ";
                    contained = true;
                    break;
                }
            }
            // -1 indicates this shape is not in the correspondence
            if (!contained) {
                os << "-1 ";
            }
        }
        // one line for each correspondence
        os << endl;
    }
}


///////////////////////////////////////////////////////////////////////////////
void SceneWriterReader::exportPointCorrespondences(HashMap<PointCorrespondenceData*, bool>& correspondences,
                                             vector<Shape*>&                                shapesOrderedById,
                                             string                                         filename)
{
    ofstream os(filename);
    
    // basic information, point correspondences, number of shapes
    // number of correspondences
    os << "P " << " " << shapesOrderedById.size() << " " << correspondences.size() << endl;
    
    exportCorrespondences((HashMap<CorrespondenceData*, bool>&) correspondences, shapesOrderedById, os);
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void SceneWriterReader::exportFaceCorrespondences(HashMap<FaceCorrespondenceData*, bool>&   correspondences,
                                            vector<Shape*>&                                 shapesOrderedById,
                                            string                                          filename)
{
    ofstream os(filename);
    
    // basic information, face correspondences, number of shapes
    // number of correspondences
    os << "F " << shapesOrderedById.size() << " " << correspondences.size() << endl;
    
    exportCorrespondences((HashMap<CorrespondenceData*, bool>&) correspondences, shapesOrderedById, os);
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void SceneWriterReader::importCorrespondences(string                            filename,
                                      int&                                      lastInsertCorrespondenceID_,
                                      vector<PointCorrespondenceData*>&         pointCorrespondences,
                                      vector<FaceCorrespondenceData*>&          faceCorrespondences,
                                      vector<Shape*>&                           shapesOrderedById,
                                      QWidget*                                  parentWidget)
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
    

    // stop if there not enough shapes to match
    if (shapesOrderedById.size() < numberOfShapes) {
        //TODO throw reader exception here, catch by main program
        return;
    }
    
    QStringList labels;
    // add all shapes that have not been chosen yet
    for (int j = 0; j < shapesOrderedById.size(); j++) {
        QString label = QString::number(shapesOrderedById[j]->getId());
        label.append(QString::fromStdString(":"+shapesOrderedById[j]->getName()));
        labels << label;
    }
    
    vector<Shape*> correspondingShapes = vector<Shape*>(0);
    
    // user chooses numberOfShapes many shapes to correspond
    unsigned counter = 1;
    for (int i = 0; i < numberOfShapes; i++) {        
        bool ok;
        QString chosen = QInputDialog::getItem(parentWidget,
                                               QObject::tr("Choose a shape:"),
                                               QString(QObject::tr("Shape for column ")).append(QString::number(counter)),
                                               labels,
                                               0,
                                               true,
                                               &ok);
        if(!ok) {
            return;
        }
        
        Shape* shape = nullptr;
        // add chosen shape to vector
        for(int j = 0; j < shapesOrderedById.size(); j++) {
            vtkIdType shapeId = chosen.split(':')[0].toInt();
            if(shapesOrderedById[j]->getId() == shapeId) {
                shape = shapesOrderedById[j];
                break;
            }
        }
        
        correspondingShapes.push_back(shape);
        // delete chosen shape from map, so it can not be chosen again
        labels.removeOne(chosen);
        counter++;
    }
    
    // create correspondeneces
    for (int i = 0; i < numberOfCorrespondences; i++) {
        CorrespondenceData* data;
        if (type == 'F') {
            data = new FaceCorrespondenceData(lastInsertCorrespondenceID_);
        } else {
            data = new PointCorrespondenceData(lastInsertCorrespondenceID_);
        }
        
        stringstream ss;
        getline(is, line);
        ss << line;
        createCorrespondenceData(correspondingShapes, ss, data);
        
        if (type == 'F') {
            faceCorrespondences.push_back((FaceCorrespondenceData*) data);
        } else {
            pointCorrespondences.push_back((PointCorrespondenceData*) data);
        }
        lastInsertCorrespondenceID_++;
    }
}

///////////////////////////////////////////////////////////////////////////////
void SceneWriterReader::createCorrespondenceData(vector<Shape*>& shapes, stringstream& ss, CorrespondenceData* data) {
    for (auto it = shapes.begin(); it != shapes.end(); it++) {
        int id;
        ss >> id;
        if (id > -1) {
            data->addShape((*it)->getId(), id);
        }
    }
}