/*
 * file:                  Importer.h
 * description:           Imports regular grid and triangulations into VTK.
 * author:                Julien Tierny <tierny@telecom-paristech.fr>.
 * date:                  July 2014.
 */

#include <Debug.h>
#include <vtkType.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h> 
#include <vtkPointData.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>

class Importer{
  
  public:
    
    Importer();
    
    ~Importer();
    
    int importTriangulation(const string &fileName);
    
    int importRegularGrid(const vector<string> &fileNames);
    
    int setOutputFileName(const string &fileName);
    
    
  protected:
    
    int readRegularGrid(const vector<string> &fileNames);
    
    int readTriangulation(const string &fileName);
    
    int writeVtkRegularGrid(const string &fileName);
    
    int writeVtkTriangulation(const string &fileName);
   
    
  private:
    
    bool                  isTriangulation_, isGrid_;
    int                   dimension_;
    vector<int>           gridSize_;
    int                   numberOfComponents_;
    vector<vector<double> > vertexData_;
    vector<vector<int> >  cellData_;
    string                outputFileName_;
    
};
