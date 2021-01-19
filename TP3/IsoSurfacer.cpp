/*
  IsoSurfacer.cpp: Isosurface computation class.
  Copyright (C) 2013  Julien Tierny <tierny@telecom-paristech.fr>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include  "IsoSurfacer.h"
#include <algorithm>
#include <iostream>

// vtkCxxRevisionMacro(IsoSurfacer, "$Revision$");
vtkStandardNewMacro(IsoSurfacer);

IsoSurfacer::IsoSurfacer(){
  Input = NULL;
  Output = NULL;
  pointSet_ = NULL;
  cellArray_ = NULL;
  fakeScalars_ = NULL;
  Type = SIMPLE;
}

IsoSurfacer::~IsoSurfacer(){

  if(Output)
    Output->Delete();
  if(pointSet_)
    pointSet_->Delete();
  if(cellArray_)
    cellArray_->Delete();
  if(fakeScalars_)
    fakeScalars_->Delete();
}

int IsoSurfacer::ComputePartialIntersection(const int &tetId){

  return 0;
}

int IsoSurfacer::ComputeSimpleIntersection(vtkCell *tet){
    
    pair<vtkIdType, vtkIdType> edge; 
    vector<double> pt_intersection(3); 
    vtkIdList* pointIds = vtkIdList::New();
    int id = 0; 
    vector<pair<vtkIdType,vtkIdType> > edge_intersected; 
    
    for(int i = 0; i< tet->GetNumberOfEdges(); i++) // on parcourt tout les edges du tetrahedron
        if (IsCellOnLevelSet(tet->GetEdge(i))) //si on a une intersection
        {
            edge.first = tet->GetEdge(i)->GetPointId(0); // Id point a de l'arête
            edge.second = tet->GetEdge(i)->GetPointId(1); // Id point b de l'arête
            edge_intersected.push_back(edge); //on stocke les points {a,b} des edges qui sont intersecté
        }
        
    ReOrderTetEdges(edge_intersected); 
    
    for(int i = 0; i<edge_intersected.size(); i++) // on calcul tout les points d'intersection
    {
        pt_intersection = ComputeEdgeIntersection(edge_intersected[i]);
        id = Output->GetPoints()->InsertNextPoint(pt_intersection[0],pt_intersection[1],pt_intersection[2]);  
        pointIds->InsertNextId(id); // ajoute l'id du point pour pouvoir créer le polygone à la fin 
    }
        
    Output->InsertNextCell(VTK_POLYGON, pointIds); 
    
    pointIds->Delete();
    return 0;
}

int IsoSurfacer::FastExtraction(){

  return 0;
}

int IsoSurfacer::ReOrderTetEdges(vector<pair<vtkIdType, vtkIdType> > &edgeList) const{
    
    pair<vtkIdType, vtkIdType> temp; 
    cout<< "ReOrderTetEdges"<<endl;
    int cpt = 1;
    //swap std
    while(cpt != 0){
        for(int i = 0; i<edgeList.size(); i++){
            vtkIdType e1_f = edgeList[i].first;
            vtkIdType e1_s = edgeList[i].second;
            vtkIdType e2_f = edgeList[i+1].first;
            vtkIdType e2_s = edgeList[i+1].second;
            if(e1_f!=e2_s && e1_f!=e2_f && e1_s!=e2_s && e1_s!=e2_f){
                cout << "la"<< endl;
                temp = edgeList[i];
                std::swap(&edgeList[i],&edgeList[i+1]);
                cpt -= 1;
            }
        }
        if(cpt <=0){
            cpt = 1;
        }
        else
            cpt = 0;
    }
    return 0;
}

int IsoSurfacer::SimpleExtraction(){
    
    vtkCell* cell; 
    for(int i = 0; i < Input->GetNumberOfCells(); i++){
        cell = Input->GetCell(i);
        if(IsCellOnLevelSet(cell))
            ComputeSimpleIntersection(cell);
    }
    
    cell->Delete();
    return 0;
}

int IsoSurfacer::StandardExtraction(){
  
  return 0;
}

void IsoSurfacer::Update(){

  if(!Input){
    cerr << "[IsoSurfacer] No input defined..." << endl;
    return;
  }


  if(pointSet_)
    pointSet_->Delete();
  pointSet_ = vtkPoints::New();
  
  
  if(cellArray_)
    cellArray_->Delete();
  cellArray_ = vtkCellArray::New();
  
  if(Output)
    Output->Delete();
 
  if(!fakeScalars_)
    fakeScalars_ = vtkDoubleArray::New();
  
  Output = vtkPolyData::New();
  Output->SetPoints(pointSet_);
  Output->SetPolys(cellArray_);
  Output->GetPointData()->SetScalars(fakeScalars_);

  scalarField_ = Input->GetPointData()->GetScalars();
 
  DebugMemory memory;
  DebugTimer  timer;
  
  switch(Type){
    
    case SIMPLE:
      cout << "[IsoSurfacer] Using simple implementation..." << endl;
      SimpleExtraction();
      break;
      
    case STANDARD:
      cout << "[IsoSurfacer] Using standard implementation..." << endl;
      StandardExtraction();
      break;
      
    case FAST:
      cout << "[IsoSurfacer] Using fast implementation..." << endl;
      FastExtraction();
      break;
  }
  
  cout << setprecision(4);
 
  cout << "[IsoSurfacer] IsoSurface extracted ("
    << Output->GetNumberOfPoints() << " vertices, "
    << Output->GetNumberOfCells() << " faces)." << endl;
  cout << "[IsoSurfacer] Extraction performed in "
    << timer.getElapsedTime() << " s. (memory usage: "
    << memory.getInstantUsage() << " MB)." << endl;
    
  // add a scalar value to the isosurface to make sure it gets colored by the
  // rest of the pipeline
  fakeScalars_->SetNumberOfTuples(Output->GetNumberOfPoints());
  for(int i = 0; i < Output->GetNumberOfPoints(); i++)
    fakeScalars_->SetComponent(i, 0, Value);
}

