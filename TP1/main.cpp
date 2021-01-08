// Fait par Clément Apavou 
#include <iostream>
#include <vtkPoints.h>
#include <vtkIdList.h>
#include <vtkUnstructuredGrid.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h> 
#include <vtkObjectFactory.h> 
#include <vtkProperty.h> 
#include <vtkGeometryFilter.h> 
#include <vtkPolyDataNormals.h> 
#include <vtkFeatureEdges.h> 
#include <vtkTubeFilter.h> 
#include <vtkPolyDataMapper.h> 

#include <vtkAutoInit.h>
#include <vtkInteractorStyleSwitchBase.h>

using namespace std;

// update state
bool isTransparent_ = false; 
bool isMesh_ = false; 
bool isEdge_ = false;

// global variable for the class
vtkActor* actor_ = NULL; 
vtkRenderWindow* window_ = NULL; 
vtkActor* actor2_ = NULL; 
vtkPolyDataMapper* data_mapper_ = NULL; 
vtkTubeFilter* tubefilter_ = NULL; 

// Q19
class CustomInteractor : public vtkInteractorStyleTrackballCamera{
public: 
    static CustomInteractor* New();
    void OnKeyPress();
  
};

void CustomInteractor::OnKeyPress(){
    string key = Interactor->GetKeySym(); 
    cout << "Tubes have now " << tubefilter_->GetOutput()->GetNumberOfPoints() << " points. " <<endl; 
    
    // Q20 & Q21
    // Q20 - lorsqu'on appuie sur t l'opacité du tétraèdre change
    // Q21 - l'opacité change selon la valeur de isTransparent_, on peut changer puis remettre l'opacité
    if(key == "t"){
        if(isTransparent_ == true){
            actor_->GetProperty()->SetOpacity(1); 
            isTransparent_ = false; 
        }
        else{
            actor_->GetProperty()->SetOpacity(0.5);
            isTransparent_ = true; 
        }
    }

    // Q22
    // Q22 - en appuyant sur m on peut afficher le maillage du tétraèdre 
    if(key == "m"){ 
        if(isMesh_ == false){
            actor_->GetProperty()->SetRepresentationToWireframe();
            isMesh_ = true; 
        }
        else{ 
            actor_->GetProperty()->SetRepresentationToSurface(); 
            isMesh_ = false; 
        }
    }
    
    // Q25
    // Q25 - en appuyant sur g on peut afficher les tubes représentant les bords du tétraèdre 
    if(key == "g"){
        if(isEdge_ == false){
            data_mapper_->SetInputData(tubefilter_->GetOutput());
            actor2_->SetMapper(data_mapper_);
            isEdge_ = true; 
        }
        else{ 
            actor2_->SetMapper(NULL);
            isEdge_ = false; 
        }
    }

    window_->Render();
    cout << "Tubes have now " << tubefilter_->GetOutput()->GetNumberOfPoints() << " points. " <<endl; 
    vtkInteractorStyle::OnKeyPress();
}

VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingOpenGL2);
vtkStandardNewMacro(CustomInteractor);

int main(int argc, char **argv){
    // Q9
    vtkPoints *pointSet_ = NULL;
    pointSet_ = vtkPoints::New();

    // Q10
    vtkIdType id1 = pointSet_->InsertNextPoint(0,0,1);
    vtkIdType id2 = pointSet_->InsertNextPoint(0,1,0);
    vtkIdType id3 = pointSet_->InsertNextPoint(1,0,0);
    vtkIdType id4 = pointSet_->InsertNextPoint(0,0,0);
    vtkIdType id5 = pointSet_->InsertNextPoint(0,0,-1);

    // Q11
    vtkIdList* list_ = NULL;
    list_ = vtkIdList::New();
    list_->InsertNextId(id1);
    list_->InsertNextId(id2);
    list_->InsertNextId(id3);
    list_->InsertNextId(id4);

    // Q12
    vtkUnstructuredGrid* tetrahedra = NULL;
    tetrahedra =  vtkUnstructuredGrid::New();
    tetrahedra->SetPoints(pointSet_); // Link vtkUnstructuredGrid object to the 3D point 
    tetrahedra->InsertNextCell(VTK_TETRA,list_); // Insert a cell of type VTK_TETRA 
    
    // Q13
    vtkIdList* list2_ = NULL;
    list2_ = vtkIdList::New();
    list2_->InsertNextId(id5);
    list2_->InsertNextId(id2);
    list2_->InsertNextId(id3);
    list2_->InsertNextId(id4);

    tetrahedra->InsertNextCell(VTK_TETRA,list2_); // Insert a cell of type VTK_TETRA 
    
    // Q14
    // vtkRenderWindow* window_ = NULL;
    window_ = vtkRenderWindow::New(); 

    // window_->Render();
    
    // la fenêtre s'ouvre puis se referme, elle n'affiche rien
    
    // Q15
    vtkRenderer* render_ = NULL;
    render_ = vtkRenderer::New();

    vtkDataSetMapper* map_ = NULL;
    map_ = vtkDataSetMapper::New();
    
    // vtkActor* actor_ = NULL;
    actor_ = vtkActor::New();
    
    map_->SetInputData(tetrahedra); 
    actor_->SetMapper(map_); 
    render_->AddActor(actor_);
  
    window_->AddRenderer(render_);

    // window_->Render();

    // la fenêtre s'ouvre puis se referme, elle affiche un tétraedre

    // Q16
    vtkRenderWindowInteractor* interactor_ = NULL;
    interactor_ = vtkRenderWindowInteractor::New();

    interactor_->SetRenderWindow(render_->GetRenderWindow());

    // window_->Render();
    // interactor_->Start(); 

    // la fenêtre s'ouvre, elle affiche un tétraedre qu'on peut manipuler à l'aide de la souris

    // Q17
    vtkInteractorStyleTrackballCamera* style_ = NULL; 
    style_ =  vtkInteractorStyleTrackballCamera::New(); 
    interactor_->SetInteractorStyle(style_);

    // window_->Render();
    // interactor_->Start(); 
    
    // la fenêtre s'ouvre, elle affiche un tétraedre qu'on peut manipuler à l'aide de la souris

    // Q18
    // On peut créer une classe qui hérite de vtkInteractorStyleTrackballCamera pour redéfinir les keyboard events
    
    // Q19
    CustomInteractor* style2_ = NULL; 
    style2_ =  CustomInteractor::New();
    interactor_->SetInteractorStyle(style2_);

    // Q23
    vtkGeometryFilter* filter_ = NULL; 
    filter_ = vtkGeometryFilter::New(); 
    filter_->SetInputData(tetrahedra); // The first filter of the pipeline 
    filter_->Update();
    
    vtkPolyDataNormals* normals_ = NULL; 
    normals_ = vtkPolyDataNormals::New();
    normals_->SetInputConnection(filter_->GetOutputPort());
    normals_->Update();
    
    vtkFeatureEdges* edges_ = NULL; 
    edges_ = vtkFeatureEdges::New();
    edges_->SetInputConnection(normals_->GetOutputPort());
    edges_->Update();
    
    // vtkTubeFilter* tubefilter_ = NULL; 
    tubefilter_ = vtkTubeFilter::New();
    tubefilter_->SetInputConnection(edges_->GetOutputPort()); 
    tubefilter_->SetRadius(0.025);
    tubefilter_->SetNumberOfSides(10);
    tubefilter_->Update();

    // Q24
    // vtkPolyDataMapper* data_mapper_ = NULL;
    data_mapper_ = vtkPolyDataMapper::New();

    // vtkActor* actor2_ = NULL;
    actor2_ = vtkActor::New();

    render_->AddActor(actor2_);
    // actor2_->SetMapper(data_mapper_);
    // data_mapper_->SetInputData(tubefilter_->GetOutput()); // The first filter of the pipeline 

    interactor_->Start(); 

    (*pointSet_).Delete();
    (*list_).Delete();
    (*list2_).Delete();
    (*tetrahedra).Delete();
    (*window_).Delete();
    (*render_).Delete();
    (*actor_).Delete();
    (*map_).Delete();
    (*interactor_).Delete();
    (*style_).Delete();
    (*style2_).Delete();
    (*filter_).Delete();
    (*normals_).Delete();
    (*edges_).Delete();
    (*tubefilter_).Delete();
    (*data_mapper_).Delete();
    (*actor2_).Delete();
    return 0;
}
