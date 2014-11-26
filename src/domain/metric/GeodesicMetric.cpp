//
//  GeodesicMetric.cpp
//  ShapeAnalyzer
//
//  Created by Zorah on 27.05.14.
//
//

#include "GeodesicMetric.h"

///////////////////////////////////////////////////////////////////////////////
// Constructors and Destructor
///////////////////////////////////////////////////////////////////////////////
metric::GeodesicMetric::GeodesicMetric(Shape* shape) throw(geodesic_error) : Metric(shape) {
    points_ = new geodesicPoints(shape_->getPolyData());
    faces_ = new geodesicFaces(shape_->getPolyData());
    
    try {
        mesh_.initialize_mesh_data(*points_, *faces_);		//create internal mesh data structure including edges
    } catch (geodesic_error& e) {
        throw e;
    }
    algorithm_ = new GeodesicAlgorithmExact(&mesh_);
}

///////////////////////////////////////////////////////////////////////////////
metric::GeodesicMetric::~GeodesicMetric() {
    delete algorithm_;
    delete points_;
    delete faces_;
}


///////////////////////////////////////////////////////////////////////////////
// Implemented Metric Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Returns a vector with all distances on the shape to the source s ordered
// by their id
// Notice that this will remove the current sources and precomputed information
vtkSmartPointer<vtkDoubleArray> metric::GeodesicMetric::getAllDistances(vtkIdType s) throw(geodesic_error) {
    // argument check
    if(s >= points_->size()) {
        throw invalid_argument("GeodesicMetric::getAllDistances: Source point (" + to_string(s) + ") larger than number of points (" + to_string(points_->size()) + ").");
    }
    
    // initialize algorithm for this source
    SurfacePoint source(&mesh_.vertices()[s]);
    vector<SurfacePoint> all_sources(1, source);
    try {
        algorithm_->propagate(all_sources);
    } catch (geodesic_error& e) {
        throw e;
    }
    
    vtkSmartPointer<vtkDoubleArray> distances = vtkSmartPointer<vtkDoubleArray>::New();
    distances->SetNumberOfValues(shape_->getPolyData()->GetNumberOfPoints());
    for(vtkIdType i = 0; i< mesh_.vertices().size(); ++i) {
        geodesic::SurfacePoint p(&mesh_.vertices()[i]);
        
        double distance;
        try {
            algorithm_->best_source(p, distance); //for a given surface point, find closets source and distance to this source
        } catch (geodesic_error& e) {
            throw e;
        }
        
        distances->SetValue(i, distance);
    }
    
    return distances;
}

///////////////////////////////////////////////////////////////////////////////
// Returns distance between a and b
// Notice that this will remove the current sources and precomputed information
// about these
double metric::GeodesicMetric::getDistance(vtkIdType a, vtkIdType b)  throw(geodesic_error) {
    SurfacePoint source(&mesh_.vertices()[a]);
    SurfacePoint target(&mesh_.vertices()[b]);
    
    vector<SurfacePoint> path;
    
    try {
        algorithm_->geodesic(source, target, path);
    } catch (geodesic_error& e) {
        throw e;
    }
    
    return length(path);
}


///////////////////////////////////////////////////////////////////////////////
// Returns the point that is farthest away from all points in sources
// Notice that this will remove the current sources and precomputed information
// about these if the input list is not the same as the current source list
vtkIdType metric::GeodesicMetric::getFarthestPoint(vtkSmartPointer<vtkIdList> sources) throw(geodesic_error) {
    vector<SurfacePoint> all_sources;
    all_sources.resize(sources->GetNumberOfIds());
    
    // create SurfacePoints for all ids
    for(int i = 0; i < sources->GetNumberOfIds(); i++) {
        SurfacePoint source(&mesh_.vertices()[sources->GetId(i)]); //create source
        all_sources[i] = source;
    }
    
    // recomputation of geodesics
    try {
        algorithm_->propagate(all_sources);
    } catch (geodesic_error& e) {
        throw e;
        return 0;
    }
    
    double maxDist = 0.0;
    
    vtkIdType id = 0;
    
    // iterate over all points
    for(int i = 0; i < mesh_.vertices().size(); i++) {
        SurfacePoint target(&mesh_.vertices()[i]); //create source
        
        // calculate shortest distance to some source
        double dist;
        try {
            algorithm_->best_source(target, dist);
        } catch (geodesic_error& e) {
            cout << e.what() << '\n';
            return 0;
        }
        
        // check if shortest distance is the greatest so far
        if(dist > maxDist) {
            maxDist = dist;
            id = i;
        }
        
    }
    
    return id;
}

///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkIntArray> metric::GeodesicMetric::getVoronoiCells(vtkSmartPointer<vtkIdList> seeds)  throw(geodesic_error) {
    vtkSmartPointer<vtkIntArray> voronoiCells = vtkSmartPointer<vtkIntArray>::New();
    voronoiCells->SetNumberOfValues(shape_->getPolyData()->GetPoints()->GetNumberOfPoints());
    
    
    vector<SurfacePoint> all_sources;
    all_sources.resize(seeds->GetNumberOfIds());
    
    // create SurfacePoints for all ids
    for(int i = 0; i < seeds->GetNumberOfIds(); i++) {
        SurfacePoint source(&mesh_.vertices()[seeds->GetId(i)]); //create source
        all_sources[i] = source;
    }
    
    // recomputation of geodesics
    try {
        algorithm_->propagate(all_sources);
    } catch (geodesic_error& e) {
        throw e;
        return voronoiCells;
    }
    
    double dist;
    
    for(vtkIdType i = 0; i < shape_->getPolyData()->GetPoints()->GetNumberOfPoints(); i++) {
        SurfacePoint p(&mesh_.vertices()[i]);
        voronoiCells->SetValue(i, algorithm_->best_source(p, dist));
    }
    
    return voronoiCells;
}



