#include <vtkVersion.h>
#include <vtkArrowSource.h>
#include <vtkCellArray.h>
#include <vtkGlyph2D.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkImageSliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkInteractorStyleImage.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>

int main(int, char *[])
{
  // Create an image
  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();

  // Specify the size of the image data
  image->SetDimensions(50,50,1);

#if VTK_MAJOR_VERSION <= 5
  image->SetNumberOfScalarComponents(2);
  image->SetScalarTypeToFloat();
  image->AllocateScalars();
#else
  image->AllocateScalars(VTK_FLOAT,2);
#endif
  int* dims = image->GetDimensions();

  // Zero the image
  for (int y = 0; y < dims[1]; y++)
    {
    for (int x = 0; x < dims[0]; x++)
      {
      float* pixel = static_cast<float*>(image->GetScalarPointer(x,y,0));
      pixel[0] = 0.0;
      pixel[1] = 0.0;
      }
    }

  {
  float* pixel = static_cast<float*>(image->GetScalarPointer(20,20,0));
  pixel[0] = -10.0;
  pixel[1] = 5.0;
  }

  {
  float* pixel = static_cast<float*>(image->GetScalarPointer(30,30,0));
  pixel[0] = 10.0;
  pixel[1] = 10.0;
  }
  
  // A better way to do this is (should be tested for compilablilty and correctness):
  // image->GetPointData()->SetActiveVectors(image->GetPointData()->GetScalars()->GetName().c_str());
  image->GetPointData()->SetActiveVectors("ImageScalars");

  // Setup the arrows
  vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
  arrowSource->Update();
  
  vtkSmartPointer<vtkGlyph2D> glyphFilter = vtkSmartPointer<vtkGlyph2D>::New();
  glyphFilter->SetSourceConnection(arrowSource->GetOutputPort());
  glyphFilter->OrientOn();
  glyphFilter->SetVectorModeToUseVector();
#if VTK_MAJOR_VERSION <= 5
  glyphFilter->SetInputConnection(image->GetProducerPort());
#else
  glyphFilter->SetInputData(image);
#endif
  glyphFilter->Update();

  // Create actors
  vtkSmartPointer<vtkImageSliceMapper> imageMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
#if VTK_MAJOR_VERSION <= 5
  imageMapper->SetInputConnection(image->GetProducerPort());
#else
  imageMapper->SetInputData(image);
#endif

  vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
  imageSlice->SetMapper(imageMapper);
  
  vtkSmartPointer<vtkPolyDataMapper> vectorMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vectorMapper->SetInputConnection(glyphFilter->GetOutputPort());
  vtkSmartPointer<vtkActor> vectorActor = vtkSmartPointer<vtkActor>::New();
  vectorActor->SetMapper(vectorMapper);

  // Setup renderer
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->AddViewProp(imageSlice);
  renderer->AddViewProp(vectorActor);
  renderer->ResetCamera();

  // Setup render window
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);

  // Setup render window interactor
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
  //renderWindowInteractor->SetInteractorStyle(style);

  // Render and start interaction
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->Initialize();

  renderWindowInteractor->Start();
  
  return EXIT_SUCCESS;
}
