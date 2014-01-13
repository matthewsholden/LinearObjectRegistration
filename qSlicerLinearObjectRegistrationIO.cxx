/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes

// SlicerQt includes
#include "qSlicerLinearObjectRegistrationIO.h"

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLLORLinearObjectCollectionNode.h"

// VTK includes
#include <vtkSmartPointer.h>
#include "vtkXMLDataParser.h"

//-----------------------------------------------------------------------------
class qSlicerLinearObjectRegistrationIOPrivate
{
public:
  bool notUsed;
};

//-----------------------------------------------------------------------------
qSlicerLinearObjectRegistrationIO::qSlicerLinearObjectRegistrationIO( QObject* _parent )
  : Superclass(_parent)
  , d_ptr(new qSlicerLinearObjectRegistrationIOPrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerLinearObjectRegistrationIO::~qSlicerLinearObjectRegistrationIO()
{
}

//-----------------------------------------------------------------------------
QString qSlicerLinearObjectRegistrationIO::description() const
{
  return "Linear Object Collection";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerLinearObjectRegistrationIO::fileType() const
{
  return QString("Linear Object Collection");
}

//-----------------------------------------------------------------------------
QStringList qSlicerLinearObjectRegistrationIO::extensions() const
{
  return QStringList() << "Linear Object Collection (*.xml)";
}

//-----------------------------------------------------------------------------
bool qSlicerLinearObjectRegistrationIO::load(const IOProperties& properties)
{
  Q_D(qSlicerLinearObjectRegistrationIO);
  Q_ASSERT( properties.contains("fileName") );
  QString fileName = properties["fileName"].toString();
  
  vtkSmartPointer< vtkMRMLLORLinearObjectCollectionNode > importCollectionNode;
  importCollectionNode.TakeReference( vtkMRMLLORLinearObjectCollectionNode::SafeDownCast( this->mrmlScene()->CreateNodeByClass( "vtkMRMLLORLinearObjectCollectionNode" ) ) );
  importCollectionNode->SetScene( this->mrmlScene() );
  this->mrmlScene()->AddNode( importCollectionNode );
  
  vtkSmartPointer< vtkXMLDataParser > parser = vtkSmartPointer< vtkXMLDataParser >::New();
  parser->SetFileName( fileName.toStdString().c_str() );
  parser->Parse();

  importCollectionNode->FromXMLElement( parser->GetRootElement() );

  return true; // TODO: Check to see read was successful first
}