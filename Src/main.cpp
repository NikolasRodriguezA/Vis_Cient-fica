#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkGlyph3DMapper.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

#include <vtkImageReader.h>
#include <vtkImageReader2Factory.h>
#include <vtkTransformTextureCoords.h>

#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPCANormalEstimation.h>
#include <vtkPointSource.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkCylinderSource.h>
#include <vtkPolyDataNormals.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkGlyph3D.h>
#include <vtkReverseSense.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkTexture.h>
#include <vtkTexturedSphereSource.h>
#include <vtkOBJReader.h>
// INCLUDES MISSING TODO
#include "iostream"
#include "string"

#include <iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<vector>
#include <vtkLookupTable.h>

#define Archivo "../Img/Data.txt"


#define vtkSPtr vtkSmartPointer
#define vtkSPtrNew(Var, Type) vtkSPtr<Type> Var = vtkSPtr<Type>::New();

using namespace std;



int counter = 0;
int contador = 0;
int count = 0;
double alt;
bool flag = true;
double wRadius = 6.371;
double coordenadas[73][3];
double densidad[73];
double pendiente;
double altura;
double alturas[73]; //Alturas Cilindros normales a   la esfera
double alt_color[73];


vtkSPtrNew(renderWindow, vtkRenderWindow);
vtkSPtrNew(renderer, vtkRenderer);
vtkNew<vtkNamedColors> colors;

double get_altura(double density)
{
	double x1 = 0;
	double x2 = 22;
	double y1 = 0.2;
	double y2 = 2.4; 

	pendiente = (y2-y1)/(x2-x1);

	// Y = mx - mx2 + y1
	altura = (pendiente*density) - (pendiente*x1) + 0.5;

	return altura;
}

double* get_colorn(double density)
{
	double* colores = new double[3];
	double x1 = 0;
	double x2 = 3;
	double y1 = 255;
	double y2 = 0; 

	pendiente = (y2-y1)/(x2-x1);

	// Y = mx - mx2 + y1
	altura = (pendiente*density) - (pendiente*x1) + y1;
	cout << "LLLLLLLLLLLLLLLLLLLLLLLL " << endl;
	cout << "COLOOOOR: " << altura << endl;
	cout << "Alturaaaaaaa : " << density << endl;

	colores[0] = 255/255;
	colores[1] = (altura/255);
	colores[2] = 0/255;

	return colores;
}

namespace {
void MakeGlyphs(vtkPolyData* src, double size, vtkGlyph3D* glyph);
}


double RadiansToDegrees(double degree){
	return (degree * 3.14159)/180.0;
}

 
double* GetCartesianPos(double r, double lat, double latCorrection, double lon , double lonCorrection, double h) {
	double* coordinates  = new double [3];


	coordinates[0] = (r+h)*cos(RadiansToDegrees(lat+latCorrection))*cos(RadiansToDegrees(lon+lonCorrection));
	coordinates[1] = (r+h)*cos(RadiansToDegrees(lat+latCorrection))*sin(RadiansToDegrees(lon+lonCorrection));
	coordinates[2] = (r+h)*sin(RadiansToDegrees(lat+latCorrection));
	return  coordinates;
}

void keyboardCallbackFunction (vtkObject *caller, unsigned long eventId, void *clientData, void *callData) {

	vtkSPtrNew(interactor, vtkRenderWindowInteractor);
	interactor = static_cast<vtkRenderWindowInteractor *>(caller);
	string keyPressed = interactor->GetKeySym ();
	
	vtkNew<vtkCylinderSource> cylinderSource;
	cylinderSource->SetCenter(0.0, 0.0, 0.0);
	cylinderSource->SetRadius(0.05);
	cylinderSource->SetHeight(alturas[counter]);
	cylinderSource->SetResolution(10);

	vtkNew<vtkPolyDataMapper> mapperc;
	mapperc->SetInputConnection(cylinderSource->GetOutputPort());

	vtkNew<vtkProperty> property;
	property->SetColor(get_colorn(alturas[counter]));

	
	vtkNew<vtkActor> actor_c;
	actor_c->SetPosition(coordenadas[counter][0], coordenadas[counter][1], coordenadas[counter][2]);
	actor_c->SetProperty(property);
	actor_c->SetMapper(mapperc);
	actor_c->RotateX(45);

	if (( keyPressed == "m" or  keyPressed == "M") and (counter < 73))
	{
		counter += 1;
		renderer->AddActor(actor_c);
		renderWindow->Render();
		
	}

	else
	{
		renderer->RemoveActor(actor_c);
		renderWindow->Render();
		interactor->Render();
	}
}



int main() {

	int counter = 0;
	int count = 1;
	int i;
	int j;
	ifstream archivo(Archivo);
	string linea;
	char delimitador = ';';

	double pos_x[73];
	double pos_y[73];
	double cluster[73];

	
	double* aux  = new double [3];
	double* aux2  = new double [3];

	int arrSize = sizeof(pos_x)/sizeof(pos_x[0]);

	getline(archivo,linea);

	while (getline(archivo, linea))
	{
		stringstream stream(linea);
		string X, Y, Cluster_id, mean_density;

		getline(stream, X, delimitador);
		getline(stream, Y, delimitador);
		getline(stream, Cluster_id, delimitador);
		getline(stream, mean_density, delimitador);

		pos_x[counter] = std::stod(X);
		pos_y[counter] = std::stod(Y);
		cluster[counter] = std::stod(Cluster_id);
		densidad[counter] = std::stod(mean_density);

		cout << "=======================" << endl;
		cout << "x: " << X << endl;
		cout << "y: " << Y << endl;
		cout << "Id: " << Cluster_id << endl;
		cout << "densidad: " << mean_density << endl;
		counter += 1;
	}

	counter = 0;

	vtkNew<vtkPoints> points;

	for (i=0; i<arrSize; i++)
	{
		//aux = GetCartesianPos(wRadius, pos_x[i], 60, pos_y[i], 170, 0.4);
		aux = GetCartesianPos(wRadius, pos_y[i], 0.4, pos_x[i], 180, 0);
		
		for (j=0; j<3; j++)
		{
			coordenadas[i][j] = aux[j];
			points->InsertNextPoint(aux);
		}

	}

	cout << arrSize << endl;

	cout << coordenadas[72][2] << endl;

	archivo.close();

	for (i=0; i<arrSize; i++)
	{
		alturas[i] = get_altura(densidad[i]);
		cout << "Densidad: " << alturas[i] << endl;
	}


	// Create a sphere with texture coordinates
	vtkNew<vtkTexturedSphereSource> source;
	source->SetThetaResolution(100);
	source->SetPhiResolution(100);
	source->SetRadius (wRadius);

	vtkNew<vtkCylinderSource> cylinderSource;
	cylinderSource->SetCenter(0.0, 0.0, 0.0);
	cylinderSource->SetRadius(0.05);
	cylinderSource->SetHeight(0.7);
	cylinderSource->SetResolution(10);

	vtkNew<vtkPolyDataMapper> mapperc;
	mapperc->SetInputConnection(cylinderSource->GetOutputPort());
	vtkNew<vtkActor> actor_c;
	actor_c->GetProperty()->SetColor(get_colorn(alturas[70]));
	actor_c->SetPosition(coordenadas[70][0], coordenadas[70][1], coordenadas[70][2]);
	actor_c->SetMapper(mapperc);
	actor_c->RotateX(45);

	vtkNew<vtkPolyData> polydata1;
  	polydata1->ShallowCopy(source->GetOutput());

	//vtkNew<vtkPolyData> polydata1;
	//polydata1->SetPoints(points);


	vtkNew<vtkDoubleArray> pointNormalsArray;
	pointNormalsArray->SetNumberOfComponents(73);
	pointNormalsArray->SetNumberOfTuples(73);

	/*for(i=0; i<arrSize; i++)
	{
		for (j=0; j<3; j++)
		{
			aux2[j] = coordenadas[i][j];
		}
		pointNormalsArray->SetTuple(i, aux2);
	}

	polydata1->GetPointData()->SetNormals(pointNormalsArray);*/

	/*vtkNew<vtkGlyph3D> glyph3D;
	glyph3D->SetSourceConnection(cylinderSource->GetOutputPort());
	glyph3D->SetVectorModeToUseNormal();
	glyph3D->SetVectorModeToUseVector();
	glyph3D->SetInputData(polydata1);
	glyph3D->SetScaleFactor(.9);
	glyph3D->Update();

	vtkNew<vtkPolyDataMapper> mapper_N;
	mapper_N->SetInputConnection(glyph3D->GetOutputPort());

	vtkNew<vtkActor> actor_N;
	actor_N->SetMapper(mapper_N);
	actor_N->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());*/
	
	// 6,371 km
	double* avion  = new double [3];

	  // Read texture file
	vtkNew<vtkImageReader2Factory> readerFactory;
	vtkSmartPointer<vtkImageReader2> imageReader;
	imageReader.TakeReference(readerFactory->CreateImageReader2("../Img/worldTopo.jpg"));
	imageReader->SetFileName("../Img/worldTopo.jpg");

	// Create texture
	vtkNew<vtkTexture> texture;
	texture->SetInputConnection(imageReader->GetOutputPort());

	vtkNew<vtkTransformTextureCoords> transformTexture;
	transformTexture->SetInputConnection(source->GetOutputPort());

	vtkNew<vtkSphereSource> sphere;
	sphere->SetPhiResolution(21);
	sphere->SetThetaResolution(21);
	sphere->SetRadius(0.04);

	vtkNew<vtkParametricSpline> spline;
	spline->SetPoints(points);

	vtkNew<vtkParametricFunctionSource> functionSource;
	functionSource->SetParametricFunction(spline);
	functionSource->Update();

	vtkNew<vtkPolyDataMapper> mapper;
  	mapper->SetInputConnection(functionSource->GetOutputPort());


	vtkNew<vtkActor> actorl;
	actorl->SetMapper(mapper);
	actorl->GetProperty()->SetColor(colors->GetColor3d("YellowGreen").GetData());
	actorl->GetProperty()->SetLineWidth(3.0);

	  // Create a polydata to store everything in
	vtkNew<vtkPolyData> polyData;
	polyData->SetPoints(points);

	vtkNew<vtkGlyph3DMapper> pointMapper;
	pointMapper->SetInputData(polyData);
	pointMapper->SetSourceConnection(sphere->GetOutputPort());

	vtkNew<vtkActor> pointActor;
	pointActor->SetMapper(pointMapper);
	pointActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());

	vtkNew<vtkPolyDataMapper> pathMapper;
	pathMapper->SetInputConnection (transformTexture->GetOutputPort ());

	vtkNew<vtkActor> pathActor;
	pathActor->SetMapper (pathMapper);
	pathActor->SetTexture(texture);


	//double r, double lat, double latCorrection, double lon , double lonCorrection, double h)
	//avion = GetCartesianPos(wRadius, 4.666, 0.4, -74.084, 180, 0.2);


	renderer->AddActor(pathActor); 
	renderer->AddActor(actor_c); 
	//renderer->AddActor(planeActor);  actor_c
	//renderer->AddActor(actor_N);
	renderer->AddActor(pointActor);
	renderer->SetBackground(colors->GetColor3d("Black").GetData());

	vtkSPtrNew(keyboardCallback, vtkCallbackCommand);
	keyboardCallback->SetCallback (keyboardCallbackFunction);

	renderWindow->AddRenderer(renderer);
	renderWindow->SetWindowName("TexturedSphere");


	vtkSPtrNew(interactor, vtkRenderWindowInteractor);
	interactor->SetRenderWindow (renderWindow);
	interactor->AddObserver (vtkCommand::KeyPressEvent, keyboardCallback);


	renderWindow->Render();
	interactor->Start();

	return EXIT_SUCCESS;

	return 0;
}


