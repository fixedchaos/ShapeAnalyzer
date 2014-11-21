//
//  RegisterCustomClasses.h
//  ShapeAnalyzer
//
//  Created by Emanuel Laude on 18.11.14.
//
//

#ifndef ShapeAnalyzer_RegisterCustomClasses_h
#define ShapeAnalyzer_RegisterCustomClasses_h

#include <memory>

#include <QWidget>

#include <vtkActor.h>

#include "Factory.h"

#include "tabs/CustomTab.h"
#include "tabs/ShapeInterpolationTab.h"
#include "tabs/MeshCheckTab.h"
#include "tabs/CorrespondenceColoringTab.h"
#include "tabs/FunctionTransferTab.h"
#include "tabs/IdentityMatchingTab.h"

#include "contextMenuItems/CustomContextMenuItem.h"
#include "contextMenuItems/ColorEigenfunctionContextMenuItem.h"
#include "contextMenuItems/ColorMetricContextMenuItem.h"
#include "contextMenuItems/ColorSignatureContextMenuItem.h"
#include "contextMenuItems/HeatDiffusionContextMenuItem.h"
#include "contextMenuItems/VoronoiCellsContextMenuItem.h"
#include "contextMenuItems/ExtractPointSegmentContextMenuItem.h"
#include "contextMenuItems/ShapeInfoContextMenuItem.h"

#include "../util/HashMap.h"

#include "../domain/Shape.h"

#include "../domain/correspondences/PointCorrespondenceData.h"
#include "../domain/correspondences/FaceCorrespondenceData.h"

#include "../domain/metric/Metric.h"
#include "../domain/metric/EuclideanMetric.h"
#include "../domain/metric/GeodesicMetric.h"

#include "../domain/signatures/Signature.h"
#include "../domain/signatures/WaveKernelSignature.h"

#include "../domain/segmentation/Segmentation.h"
#include "../domain/segmentation/VoronoiCellSegmentation.h"

typedef Factory<CustomTab, const HashMap<vtkActor*, Shape*>&, const HashMap<PointCorrespondenceData*, bool>&, const HashMap<FaceCorrespondenceData*, bool>&, QWidget*> CustomTabFactory;

typedef Factory<CustomContextMenuItem> CustomContextMenuItemFactory;


struct RegisterCustomClasses {
    static void registerTabs() {
        //tabs
        CustomTabFactory::getInstance()->Register<IdentityMatchingTab>("identity_matching", "Correspondences>>Identity Matching");
        CustomTabFactory::getInstance()->Register<ShapeInterpolationTab>("shape_interpolation", "Shapes>>Shape Interpolation");
        CustomTabFactory::getInstance()->Register<MeshCheckTab>("mesh_check", "Shapes>>Mesh Checker");
        CustomTabFactory::getInstance()->Register<CorrespondenceColoringTab>("correspondence_coloring", "Correspondences>>Correspondence Coloring");
        CustomTabFactory::getInstance()->Register<FunctionTransferTab>("function_transfer", "Correspondences>>Function Transfer");
    }
    
    static void registerContextMenuItems() {
        //menu items
        CustomContextMenuItemFactory::getInstance()->Register<ShapeInfoContextMenuItem>("shape_info", "Shape Info");
        
        CustomContextMenuItemFactory::getInstance()->Register<ColorMetricContextMenuItem<GeodesicMetric>>("color_metric_geodesic", "Coloring>>Metric>>Geodesic");
        
        CustomContextMenuItemFactory::getInstance()->Register<ColorMetricContextMenuItem<EuclideanMetric>>("color_metric_euclidean", "Coloring>>Metric>>Euclidean");
        
        CustomContextMenuItemFactory::getInstance()->Register<ColorSignatureContextMenuItem<WaveKernelSignature>>("color_signature_wavekernel", "Coloring>>Signature>>Wave Kernel");
        
        CustomContextMenuItemFactory::getInstance()->Register<VoronoiCellsContextMenuItem<GeodesicMetric>>("voronoicells_geodesic", "Segmentation>>Voronoi Cells>>Geodesic");
        
        CustomContextMenuItemFactory::getInstance()->Register<VoronoiCellsContextMenuItem<EuclideanMetric>>("voronoicells_euclidean", "Segmentation>>Voronoi Cells>>Euclidean");
        
        CustomContextMenuItemFactory::getInstance()->Register<ExtractPointSegmentContextMenuItem>("extract_point_segment", "Extract Point Segment as new Shape");
    }
};

#endif