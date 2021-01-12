/*
 * file:                  Importer.cpp
 * description:           Imports regular grid and triangulations into VTK.
 * author:                Julien Tierny <tierny@telecom-paristech.fr>.
 * date:                  July 2014.
 */

// Fait par Clément Apavou

#include <Importer.h>

Importer::Importer(){
    
    isTriangulation_ = false;
    isGrid_ = false;
    
    dimension_ = 0;
    numberOfComponents_ = 0;
    
    outputFileName_ = "output";
}

Importer::~Importer(){
  
}

int Importer::importTriangulation(const string &fileName){
    
    int readingReturn = readTriangulation(fileName);
    
    if(readingReturn != 0)
        return readingReturn;
    
    return writeVtkTriangulation(outputFileName_);
}

int Importer::importRegularGrid(const vector<string> &fileNames){

    int readingReturn = readRegularGrid(fileNames);
    
    if(readingReturn != 0)
        return readingReturn;
    
    return writeVtkRegularGrid(outputFileName_);
    
    return 0;
}

int Importer::readRegularGrid(const vector<string> &fileNames){

    Timer t;
    
    string line;
    ifstream myfile;
    myfile.open(fileNames[0].c_str());
    string type;
    int temp;
    int nb_file = 0;
    int NbvertexData_ = 1;
    
    for(int i = 1; i < 10; i++){
        if(fileNames[i]=="")
            break;
        nb_file ++;
    }
    this -> numberOfComponents_ = nb_file;
    
    if (myfile.is_open())
    {
        myfile >> type;
        myfile >> this->dimension_;  
        for(int i = 0; i< this->dimension_;i++){
            myfile >> temp;
            gridSize_.push_back(temp);
            NbvertexData_*=temp;
        }
        myfile.close();
    
        vertexData_.resize(NbvertexData_);
        for(unsigned int i = 0; i < vertexData_.size(); i++)
        {
            vertexData_[i].resize(numberOfComponents_);
        }

        if(type == "double"){
            for(int i = 0; i < numberOfComponents_; i++){
                ifstream file(fileNames[i+1]);
                vector<double> vector_double(NbvertexData_);
                file.read((char *) vector_double.data(), sizeof(double)*NbvertexData_);  
                for(int j = 0; j < NbvertexData_; j++)
                {
                    vertexData_[j][i] = vector_double[j];
                }
                file.close(); 
            }
        }
        else if(type == "unsigned_char"){
            for(int i = 0; i < numberOfComponents_; i++){
                ifstream file(fileNames[i+1]);
                vector<unsigned char> vector_unsignedchar(NbvertexData_);
                file.read((char *) vector_unsignedchar.data(), sizeof(unsigned char)*NbvertexData_);  
                for(int j = 0; j < NbvertexData_; j++)
                {
                    vertexData_[j][i] = vector_unsignedchar[j];
                }
                file.close(); 
            }
        }
        else if(type == "float"){
            for(int i = 0; i < numberOfComponents_; i++){
                ifstream file(fileNames[i+1]);
                vector<float> vector_float(NbvertexData_);
                file.read((char *) vector_float.data(), sizeof(float)*NbvertexData_);  
                for(int j = 0; j < NbvertexData_; j++)
                {
                    vertexData_[j][i] = vector_float[j];
                }
                file.close(); 
            }            
        }
    }
    else cout << "Unable to open file"<< endl; 
    
    cout << "[Importer] Input regular grid read in "
        << t.getElapsedTime() << " s." << endl;
        
    cout << "[Importer] Grid dimensions: ";
    for(int i = 0; i < dimension_; i++){
        if(!i){
        cout << gridSize_[i];
        }
        else{
        cout << "x" << gridSize_[i];
        }
    }
    cout << endl;
    cout << "[Importer] Number of data components: " << numberOfComponents_<< endl;  
        
    return 0;
}

int Importer::readTriangulation(const string &fileName){

    Timer t;
    
    string line;
    ifstream myfile;
    myfile.open(fileName);
    
    int NbvertexData_; 
    int NbcellData_;
    double vertexAttribute; 
    int vertexId; 
    
    if (myfile.is_open())
    {
        myfile >> this->dimension_;
        myfile >> this->numberOfComponents_;    
        myfile >> NbvertexData_; 
        myfile >> NbcellData_;
        
        vertexData_.resize(NbvertexData_);
        cellData_.resize(NbcellData_); 
        
        getline(myfile,line);
        
        for(int i = 0; i<NbvertexData_; i++)
        {   
            for(int j = 0; j < numberOfComponents_; j++)
            {
                myfile >> vertexAttribute;
                vertexData_[i].push_back(vertexAttribute);
            }
            getline(myfile,line);
        }
        
        for(int i = 0; i<NbcellData_; i++)
        {   
            for(int j = 0; j < dimension_+1; j++)
            {
                myfile >> vertexId;
                cellData_[i].push_back(vertexId);
            }
            getline(myfile,line);
        }
    
        myfile.close();
    }
    else cout << "Unable to open file"<< endl; 
        
    cout << "[Importer] Input triangulation read in "
        << t.getElapsedTime() << " s." << endl;
    cout << "[Importer] " << vertexData_.size() << " vertices and "
        << cellData_.size() << " cells." << endl;
    cout << "[Importer] Dimension=" << dimension_ 
        << ", NumberOfComponents=" << numberOfComponents_ << endl;
    
    return 0;
}

int Importer::setOutputFileName(const string &fileName){
  
    outputFileName_ = fileName;
    
    return 0;
}

int Importer::writeVtkRegularGrid(const string &fileName){

    Timer t;
    vtkImageData* imageData = vtkImageData::New();
    vtkDoubleArray* array_ = vtkDoubleArray::New();
    
    if(dimension_== 2)
        imageData-> SetDimensions(gridSize_[0],gridSize_[1],1); 
    else if(dimension_== 3)
        imageData-> SetDimensions(gridSize_[0],gridSize_[1],gridSize_[2]); 
    
    if(numberOfComponents_ == 2){// pour vectorFields/2-grid
        for(unsigned int i = 0; i < vertexData_.size(); i++)
            vertexData_[i].push_back(0);
        array_->SetNumberOfComponents(3); 
    }
    else
        array_->SetNumberOfComponents(numberOfComponents_); 
    
    array_->SetNumberOfTuples(vertexData_.size()); 
    
    for (unsigned int i = 0; i < vertexData_.size(); i++)   
        array_->SetTuple(i,(vertexData_[i]).data());
    
    if(numberOfComponents_ == 1)
        imageData->GetPointData()->SetScalars(array_);
    else if(numberOfComponents_ == 3 or numberOfComponents_ == 2) // == 2 pour vectorFields/2-grid
        imageData->GetPointData()->SetVectors(array_);
    else if (numberOfComponents_ == 9) 
        imageData->GetPointData()->SetTensors(array_); 
    
    vtkXMLImageDataWriter* writer_ = vtkXMLImageDataWriter::New();
    writer_->SetFileName(outputFileName_.append(".vti").c_str()); 
    writer_->SetInputData(imageData);
    writer_->Write();
  
    cout << "[Importer] VTK regular grid written in "
        << t.getElapsedTime() << " s." << endl;
        
    (*imageData).Delete();
    (*array_).Delete();
    (*writer_).Delete();
    return 0;
}


int Importer::writeVtkTriangulation(const string &fileName){
 
    Timer t;
    
    vtkUnstructuredGrid *triangulation = vtkUnstructuredGrid::New();
    vtkPoints *pointSet_ = vtkPoints::New();
    vtkIdList* list_ = vtkIdList::New();
    vtkDoubleArray* array_ = vtkDoubleArray::New();
    
    array_->SetNumberOfComponents(numberOfComponents_-3); // -3 on enlève les 3 coordonnées
    array_->SetNumberOfTuples(vertexData_.size()); 
  
    for(unsigned i = 0; i < vertexData_.size(); i++)
    {   
        pointSet_->InsertNextPoint(vertexData_[i][0],vertexData_[i][1],vertexData_[i][2]); //(x,y,z)
        for(int j = 3; j < numberOfComponents_; j++ )
            array_->SetTuple(i,&vertexData_[i][j]);
    }
    triangulation->SetPoints(pointSet_); // met les points sans les lier
    
    for(unsigned i = 0; i < cellData_.size(); i++)
    {   
        for (int j=0;j<dimension_+1;j++){
            list_->InsertNextId(cellData_[i][j]);
        }
        
        if(dimension_ == 2){
            triangulation-> InsertNextCell(VTK_TRIANGLE,list_); // lie les points
            list_->Reset();
        }
        else{ 
            triangulation-> InsertNextCell(VTK_TETRA,list_); // lie les points
            list_->Reset();
        }
    }   

    if(numberOfComponents_ == 4)
        triangulation->GetPointData()->SetScalars(array_);
    else if(numberOfComponents_ == 6) 
        triangulation->GetPointData()->SetVectors(array_);
    else if (numberOfComponents_ == 12) 
        triangulation->GetPointData()->SetTensors(array_); 
    
    vtkXMLUnstructuredGridWriter* writer_ = vtkXMLUnstructuredGridWriter::New(); 
    writer_-> SetFileName(outputFileName_.append(".vtu").c_str());
    writer_->SetInputData(triangulation);
    writer_->Write();
  
    cout << "[Importer] VTK triangulation written in "
        << t.getElapsedTime() << " s." << endl;
        
    (*triangulation).Delete();
    (*pointSet_).Delete();
    (*list_).Delete();
    (*array_).Delete();
    (*writer_).Delete();
    return 0;
}
