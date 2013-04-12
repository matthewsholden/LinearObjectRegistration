
// Qt includes
#include <QDebug>
#include <QtCore>
#include <QtGui>
#include <QTimer>

// SlicerQt includes
#include "qSlicerLinearObjectRegistrationModuleWidget.h"
#include "ui_qSlicerLinearObjectRegistrationModule.h"

#include "vtkSlicerLinearObjectRegistrationLogic.h"

#include "vtkMRMLModelNode.h"
#include "vtkMRMLNode.h"



//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerLinearObjectRegistrationModuleWidgetPrivate: public Ui_qSlicerLinearObjectRegistrationModule
{
  Q_DECLARE_PUBLIC( qSlicerLinearObjectRegistrationModuleWidget );
protected:
  qSlicerLinearObjectRegistrationModuleWidget* const q_ptr;
public:
  qSlicerLinearObjectRegistrationModuleWidgetPrivate( qSlicerLinearObjectRegistrationModuleWidget& object );
  vtkSlicerLinearObjectRegistrationLogic* logic() const;
};




//-----------------------------------------------------------------------------
// qSlicerLinearObjectRegistrationModuleWidgetPrivate methods



qSlicerLinearObjectRegistrationModuleWidgetPrivate
::qSlicerLinearObjectRegistrationModuleWidgetPrivate( qSlicerLinearObjectRegistrationModuleWidget& object )
 : q_ptr( &object )
{
}


//-----------------------------------------------------------------------------
vtkSlicerLinearObjectRegistrationLogic*
qSlicerLinearObjectRegistrationModuleWidgetPrivate::logic() const
{
  Q_Q( const qSlicerLinearObjectRegistrationModuleWidget );
  return vtkSlicerLinearObjectRegistrationLogic::SafeDownCast( q->logic() );
}





//-----------------------------------------------------------------------------
// qSlicerLinearObjectRegistrationModuleWidget methods


//-----------------------------------------------------------------------------
qSlicerLinearObjectRegistrationModuleWidget::qSlicerLinearObjectRegistrationModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerLinearObjectRegistrationModuleWidgetPrivate( *this ) )
{
  this->Timer = new QTimer( this );
  this->TimerIntervalSec = 0.1;
}


//-----------------------------------------------------------------------------
qSlicerLinearObjectRegistrationModuleWidget::~qSlicerLinearObjectRegistrationModuleWidget()
{
  delete this->Timer;
}



void qSlicerLinearObjectRegistrationModuleWidget
::OnGeometryFileButtonClicked()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );
  
  QString filename = QFileDialog::getOpenFileName( this, tr("Open geometry"), "", tr("XML Files (*.xml)") );
  
  if ( filename.isEmpty() == false )
  {
    
    QProgressDialog dialog;
    dialog.setModal( true );
    dialog.setLabelText( "Please wait while reading XML file..." );
    dialog.show();
    dialog.setValue( 10 );
    
    d->logic()->ImportGeometry( filename.toStdString() );

    dialog.close();
    
  }
  
  this->UpdateGUI();
}



void qSlicerLinearObjectRegistrationModuleWidget
::OnRecordFileButtonClicked()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );
  
  QString filename = QFileDialog::getOpenFileName( this, tr("Open record"), "", tr("XML Files (*.xml)") );
  
  if ( filename.isEmpty() == false )
  {
    
    QProgressDialog dialog;
    dialog.setModal( true );
    dialog.setLabelText( "Please wait while reading XML file..." );
    dialog.show();
    dialog.setValue( 10 );
    
    d->logic()->ImportRecord( filename.toStdString() );

    dialog.close();
    
  }
  
  this->UpdateGUI();
}



void qSlicerLinearObjectRegistrationModuleWidget
::OnRegisterButtonClicked()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );
    
  QProgressDialog dialog;
  dialog.setModal( true );
  dialog.setLabelText( "Registering..." );
  dialog.show();
  dialog.setValue( 10 );
    
  d->logic()->Register();

  dialog.close();
  
  this->UpdateGUI();
}



void qSlicerLinearObjectRegistrationModuleWidget
::OnTransformNodeSelected( vtkMRMLNode* newRegistrationTransformNode )
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );
      
  d->logic()->SetRegistrationTransformNode( vtkMRMLLinearTransformNode::SafeDownCast( newRegistrationTransformNode ) );
  
  this->UpdateGUI();
}



void
qSlicerLinearObjectRegistrationModuleWidget
::setup()
{
  Q_D(qSlicerLinearObjectRegistrationModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
  
  connect( d->GeometryFileButton, SIGNAL( clicked() ), this, SLOT( OnGeometryFileButtonClicked() ) );
  connect( d->RecordFileButton, SIGNAL( clicked() ), this, SLOT( OnRecordFileButtonClicked() ) );

  connect( d->RegisterButton, SIGNAL( clicked() ), this, SLOT( OnRegisterButtonClicked() ) );

  connect( d->TransformNodeSelector, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( OnTransformNodeSelected( vtkMRMLNode* ) ) );
}



void qSlicerLinearObjectRegistrationModuleWidget
::UpdateGUI()
{
  Q_D( qSlicerLinearObjectRegistrationModuleWidget );

  std::stringstream ss;

  ss.str( "" );
  ss << "Status: " << d->logic()->GetStatus();
  d->StatusLabel->setText( ss.str().c_str() );

  ss.str( "" );
  ss << "RMS Error: " << d->logic()->GetError();
  d->ErrorLabel->setText( ss.str().c_str() );
  
}

