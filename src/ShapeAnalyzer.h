#ifndef ShapeAnalyzer_H
#define ShapeAnalyzer_H

#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkExtractSelection.h>
#include <vtkDataArray.h>
#include <vtkDataSetMapper.h>
#include <vtkObject.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkTriangleFilter.h>
#include <vtkSelectionNode.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataNormals.h>

#include <QKeySequence>
#include <QMainWindow>
#include <QShortcut>
#include <QString>
#include <QFileDialog>
#include <QActionGroup>
#include <qinputdialog.h>


#include <unordered_map>

#include "Correspondence.h"
#include "CorrespondenceListItem.h"
#include "CorrespondencePicker.h"
#include "vtkShape.h"
#include "ShapeListItem.h"
#include "FaceCorrespondencePicker.h"
#include "PointCorrespondencePicker.h"
#include "vtkGeodesic.h"
#include "vtkOFFReader.h"
#include "vtkToscaASCIIReader.h"

#include "ui_help.h"
#include "ui_ShapeAnalyzer.h"

using namespace std;

class ShapeAnalyzer : public QMainWindow, private Ui::ShapeAnalyzer {
    Q_OBJECT

    // manages update of correspondences after scene transformation
    class vtkBoxWidgetCallback : public vtkCommand {
    public:
        ShapeAnalyzer *sa;
        
        static vtkBoxWidgetCallback *New() {
            return new vtkBoxWidgetCallback;
        }
        
        virtual void Execute(vtkObject *caller, unsigned long, void*) {
            vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
            vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
            widget->GetTransform(t);
            widget->GetProp3D()->SetUserTransform(t);

            vtkSmartPointer<vtkShape> shape = sa->findShapeByActor(reinterpret_cast<vtkActor*>(widget->GetProp3D()));

            // transform correspondences
            for(unordered_map<vtkActor*, Correspondence*>::iterator it = sa->correspondencesByActor_.begin(); it != sa->correspondencesByActor_.end(); it++) {
                if(it->second->getShape1() == shape) {
                    it->second->transform1(t);
                }
                
                if(it->second->getShape2() == shape) {
                    it->second->transform2(t);
                }
            }
            
            // transform fps
            for(unordered_map<vtkActor*, vtkSmartPointer<vtkShape> >::iterator it = sa->shapesByActor_.begin(); it != sa->shapesByActor_.end(); it++) {
                it->second->transformFPS(t);
            }
            
            sa->qvtkWidget->GetRenderWindow()->Render();
        }
    };
    
public:
    ShapeAnalyzer();
    ~ShapeAnalyzer() {
        delete correspondencePicker_;
    };
    
private slots:
    //QT slots
    virtual void slotExit();
    virtual void slotResetCamera();
    virtual void slotClear();

    virtual void slotOpenFile();

    virtual void slotOpenHelpWindow();
    virtual void slotShowContextMenuShapes(const QPoint& pos);
    virtual void slotShowContextMenuCorrespondences(const QPoint& pos);

    virtual void slotClearCurrentSelection();

    virtual void slotToggleBoxWidget();
    virtual void slotAddCorrespondencesMode();
    
    
    virtual void slotSetCurrentBoxWidget(QListWidgetItem* current, QListWidgetItem* previous);
    virtual void slotSetSelectedCurrentCorrespondence(QListWidgetItem* current, QListWidgetItem* previous);
    
    virtual void slotSetShapeDisplayMode();
    virtual void slotSetCorrespondenceType();
    
    virtual void slotSaveScene();
    virtual void slotExportScene();
    
    //vtk widget slots
    virtual void vtkClickHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command);
    virtual void vtkMouseMoveHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command);
private:
    //QT
    void qtConnectListCorrespondences();
    void qtConnectListShapes();
    bool eventFilter(QObject *object, QEvent *event);
    
    //Show context menus at global position. Either called from qt slots or from VTK widget (right click on shape/correspondence)
    void qtShowContextMenuShapes(const QPoint& pos);
    void qtShowContextMenuCorrepondences(const QPoint& pos);
    
    void qtInputDialogFPS();
    void qtInputDialogRename(QListWidgetItem* item);
    
    //vtk
    void vtkCorrespondenceClicked(Correspondence* correspondence, vtkIdType cellId, QPoint &pos, unsigned long vtkEvent, vtkCommand *command);
    void vtkShapeClicked(vtkSmartPointer<vtkShape> shape, vtkIdType cellId, QPoint &pos, unsigned long vtkEvent, vtkCommand *command);
    void vtkSetup();
    void vtkAddShape(vtkSmartPointer<vtkShape> shape);
    void vtkOpenShape(vtkPolyDataAlgorithm* reader);
    void vtkOpenScene(string filename);
    void vtkSaveScene(string filename);
    void vtkImportScene(string filename);
    void vtkExportScene(string filename);
    
    Correspondence* findCorrespondenceByActor(vtkActor* actor);
    vtkSmartPointer<vtkShape> findShapeByActor(vtkActor* actor);
    
    
    void clear();
    void clearCorrespondences();
    void deleteCorrespondence(int i);
    void deleteShape(int i);

    //index shapes & corresondences by their actors. unordered_map corresponds to hashmap. Faster access in linear time worst case. Usually constant time.
    unordered_map<vtkActor*, vtkSmartPointer<vtkShape> > shapesByActor_;
    unordered_map<vtkActor*, Correspondence*> correspondencesByActor_;

    //vtk stuff
    vtkSmartPointer<vtkRenderer> renderer_;
    vtkSmartPointer<vtkEventQtSlotConnect> connections_;
    
    
    CorrespondencePicker* correspondencePicker_;
    
    
    //QT
    QActionGroup* actionGroupCorrespondenceType;
    QActionGroup* actionGroupMode;
    QActionGroup* actionGroupShapeDisplayMode;
    
    //counter for ids
    int lastInsertShapeID_;
    int lastInsertCorresondenceID_;
};

#endif
