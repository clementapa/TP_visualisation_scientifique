/*
 * file:                  main.cpp
 * description:           Interactive program for isosurface extraction.
 * author:                Julien Tierny <tierny@telecom-paristech.fr>.
 * date:                  July 2014.
 */

#include                  <CommandLineParser.h>
#include                  <Importer.h>

int main(int argc, char **argv) {

  CommandLineParser parser;
  Importer importer;
  
  vector<string> inputGridFileName(10);
  
  parser.setStringArgument("g", &inputGridFileName[0], 
    "Path to the grid specification", true);
  
  for(int i = 0; i < 9; i++){
    
    stringstream key, description;
    key << "g" << i;
    description << "Path to the " << i << "-th grid component"; 
    
    parser.setStringArgument(key.str(), &inputGridFileName[i + 1],
      description.str(), true);
  }
  
  string inputTriangulationFileName;
  
  parser.setStringArgument("t", &inputTriangulationFileName,
    "Path to the input triangulation", true);
  
  parser.parse(argc, argv);
  
  if(inputTriangulationFileName.length()){
    // the user provided a path to a triangulation
    
    importer.importTriangulation(inputTriangulationFileName);
  }
  else if(inputGridFileName[0].length()){
    
    importer.importRegularGrid(inputGridFileName);
  }
  else{
    cerr 
      << endl
      << "ERROR! Please provide either a grid or a triangulation path!"
      << endl << endl;
    parser.printUsage(argv[0]);
  }
  
  return 0;
}
