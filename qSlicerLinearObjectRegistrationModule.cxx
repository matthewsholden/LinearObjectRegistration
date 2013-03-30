/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QtPlugin>

// ExtensionTemplate Logic includes
#include <vtkSlicerLinearObjectRegistrationLogic.h>

// ExtensionTemplate includes
#include "qSlicerLinearObjectRegistrationModule.h"
#include "qSlicerLinearObjectRegistrationModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerLinearObjectRegistrationModule, qSlicerLinearObjectRegistrationModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerLinearObjectRegistrationModulePrivate
{
public:
  qSlicerLinearObjectRegistrationModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerLinearObjectRegistrationModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerLinearObjectRegistrationModulePrivate::qSlicerLinearObjectRegistrationModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerLinearObjectRegistrationModule methods

//-----------------------------------------------------------------------------
qSlicerLinearObjectRegistrationModule::qSlicerLinearObjectRegistrationModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerLinearObjectRegistrationModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerLinearObjectRegistrationModule::~qSlicerLinearObjectRegistrationModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerLinearObjectRegistrationModule::helpText()const
{
  return "For help on how to use this module visit: <a href='https://www.assembla.com/spaces/slicerigt'>SlicerIGT</a>";
}

//-----------------------------------------------------------------------------
QString qSlicerLinearObjectRegistrationModule::acknowledgementText()const
{
  return "This work was was funded by Cancer Care Ontario and the Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO)";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLinearObjectRegistrationModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Tamas Ungi (Queen's University");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerLinearObjectRegistrationModule::icon()const
{
  return QIcon(":/Icons/LinearObjectRegistration.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerLinearObjectRegistrationModule::categories() const
{
  return QStringList() << "IGT";
}

//-----------------------------------------------------------------------------
QStringList qSlicerLinearObjectRegistrationModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerLinearObjectRegistrationModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerLinearObjectRegistrationModule::createWidgetRepresentation()
{
  return new qSlicerLinearObjectRegistrationModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerLinearObjectRegistrationModule::createLogic()
{
  return vtkSlicerLinearObjectRegistrationLogic::New();
}
