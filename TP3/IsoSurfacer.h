/*
  IsoSurfacer.h: Isosurface computation class.
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

// Fait par Clément Apavou et Arthur Zucker

#ifndef ISOSURFACER_H
#define ISOSURFACER_H

#include  <map>
#include  <queue>

#include  <Debug.h>

#include  <vtkAlgorithm.h>
#include  <vtkCellArray.h>
#include  <vtkDoubleArray.h>
#include  <vtkObjectFactory.h>
#include  <vtkPointData.h>
#include  <vtkPolyData.h>
#include  <vtkUnstructuredGrid.h>

enum extractionType {SIMPLE, STANDARD, FAST};

class TetIndex{
 
    public:
        
    TetIndex(){
    
    };
    
    
    void setBounds(float min, float max); //store the global function range (global and minimum and maximum) passed as argument
    void setResolution(int size); //sets size of the table (passed as argument)
    void addTet(float min, float max, vtkIdType IdTetra); //given the function span and the Id of a tetrahedron (passed as arguments), stores the tet Id in the table
    vector<vtkIdType>* getCandidates(float isovalue); // returns a pointer to the corresponding interval (returns a pointer of a vector of tetrahedron Ids).

    protected:
      float Fmin;
      float Fmax;
      int resolution;
      vector<vector<vtkIdType>> list_interval;
    
};

class EdgeIntersection {
  
  public:
    pair<vtkIdType, vtkIdType> Edge; 
    vtkIdType VertexId;
    
};

class VTK_EXPORT IsoSurfacer : public vtkAlgorithm {
  
  public:
   
    static IsoSurfacer* New();
    // vtkTypeRevisionMacro(IsoSurfacer, vtkObject);
    vtkTypeMacro(IsoSurfacer, vtkObject);
    
    vtkSetMacro(Value, double);
    vtkSetMacro(Input, vtkUnstructuredGrid *);
    vtkSetMacro(Type, extractionType);
    
    vtkSetMacro(Neighbors, vector<vector<vtkIdType>>*); //q18
    
    vtkSetMacro(FastIndex, TetIndex*);

    vtkGetMacro(Output, vtkPolyData *);
   
    void Update();

  protected:
    
    IsoSurfacer();
    ~IsoSurfacer();
    
    inline vector<double> ComputeEdgeIntersection(pair<vtkIdType, vtkIdType> &edge) const{
        
        // recuperation des coordonnées des deux sommets de l'arête
        double a[3];
        double b[3];
        Input->GetPoints()->GetPoint(edge.first, a);
        Input->GetPoints()->GetPoint(edge.second, b);
        
        // recuperation des valeurs scalaires
        double scalar_a; 
        scalarField_->GetTuple(edge.first,&scalar_a); 
        double scalar_b; 
        scalarField_->GetTuple(edge.second,&scalar_b); 
        
        double coeff = (Value - scalar_a)/(scalar_b - scalar_a); // pour calculer les coordonnées
        
        vector<double> p(3);
        for(int i = 0; i < p.size(); i++)
            p[i] = a[i] + coeff * (b[i]-a[i]); 
 
        return p;
    };
    
    int ComputePartialIntersection(const int &tetId);
      
    int ComputeSimpleIntersection(vtkCell *tet);
   
    int FastExtraction(); 
    
    inline bool IsCellOnLevelSet(vtkCell *cell) const{
    
        double min_pt_cell; 
        double max_pt_cell; 
        
        int vertex_nb = cell->GetNumberOfPoints(); // on recup le nb de pt dans la cellule

        double Values[vertex_nb]; 
        
        // recuperation des valeurs scalaires des points de la cellule
        for(int i=0;i<vertex_nb;i++)
            scalarField_->GetTuple(cell->GetPointIds()->GetId(i),&Values[i]);
        
        min_pt_cell = *(std::min_element(Values,Values+vertex_nb));
        max_pt_cell = *(std::max_element(Values,Values+vertex_nb));
        
        if(Value >= min_pt_cell && Value <= max_pt_cell)
            return true; 
        else 
            return false;  
    };

    int ReOrderTetEdges(vector<pair<vtkIdType, vtkIdType> > &edgeList) const;
    
    int SimpleExtraction();
    
    int StandardExtraction();
    
    // variables for get/set macros
    vtkUnstructuredGrid               *Input;
    vtkPolyData                       *Output;
    extractionType                    Type;
    double                            Value;
    // traversal speed-up q18
    vector<vector<vtkIdType>>         *Neighbors;
    
    vector<vector<EdgeIntersection>>  Edges;
    
    TetIndex                          *FastIndex;

    // internal variables
    vtkPoints                         *pointSet_;
    vtkCellArray                      *cellArray_;
    vtkDoubleArray                    *fakeScalars_;
    
    // speed up
    vtkDataArray                      *scalarField_;
    
  private:
    // not implemented
    
    IsoSurfacer(const IsoSurfacer&);
    
    void operator=(const IsoSurfacer &);
};

#endif // ISOSURFACER_H
