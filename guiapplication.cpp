#include "guiapplication.h"

// qt
#include <QOpenGLContext>
#include <QDebug>

// stl
#include <iostream>
#include <cassert>



GuiApplication::GuiApplication(int& argc, char **argv) : QGuiApplication(argc, argv) {

  setApplicationDisplayName( "Hello GMlib app display name," );

  connect( &_window, &Window::sceneGraphInitialized,
           this,     &GuiApplication::onSceneGraphInitialized,
           Qt::DirectConnection );

  connect( this, &GuiApplication::signOnSceneGraphInitializedDone,
           this, &GuiApplication::afterOnSceneGraphInitialized );

  connect( &_window, &Window::sceneGraphInvalidated,
           this,     &GuiApplication::onSceneGraphInvalidated,
           Qt::DirectConnection );

  connect( this, &QGuiApplication::lastWindowClosed,
           this, &QGuiApplication::quit );


  connect( &_window, &Window::signKeyPressed,
           this,     &GuiApplication::handleKeyPress );


  connect( &_window, &Window::beforeRendering,
           this,     &GuiApplication::handleGLInputEvents,
	   Qt::DirectConnection );

  connect( &_window, &Window::signMousePressed,
           this, &GuiApplication::handleMouseButtonPressedEvents);

  connect( &_window, &Window::signMouseReleased,
           this, &GuiApplication::handleMouseButtonReleasedEvents);

  connect( &_window, &Window::signMouseMoved,
           this, &GuiApplication::handleMouseMovementEvents);

  connect(&_window, &Window::signWheelEventOccurred,
          this,     &GuiApplication::handleWheelEvents);



  _window.setSource(QUrl("qrc:///qml/main.qml"));
  _window.show();
}


GuiApplication::~GuiApplication() {

  _scenario.stopSimulation();

  _window.releasePersistence();
  _window.releaseResources();

  std::cout << "Bye bye ^^, ~~ \"emerge --oneshot life\"" << std::endl;
}

void
GuiApplication::onSceneGraphInitialized() {

  // Init the scene object and GMlibs OpenGL backend
  _scenario.initialize();

  // Pring surface format
  qDebug() << _window.format();

  // Init a scenario
  _scenario.initializeScenario();
  _scenario.prepare();

  emit signOnSceneGraphInitializedDone();
}

void
GuiApplication::afterOnSceneGraphInitialized() {

  // Start simulator
  _scenario.startSimulation();
}

void GuiApplication::onSceneGraphInvalidated() {

  _scenario.deinitialize();
}

void GuiApplication::handleKeyPress( QKeyEvent* e ) {

  if(e->key() == Qt::Key_Q) {
    _window.close();
  }

  else
  {
      //if(e->key() == Qt::Key_P)
      //{
        _input_events.push(std::make_shared<QKeyEvent>(*e));
      //}
  }

}

void GuiApplication::handleGLInputEvents() {

  while(!_input_events.empty())
  {
    const auto& e  = _input_events.front();
    const auto& ke = std::dynamic_pointer_cast<const QKeyEvent>(e);

    if(ke and ke->key() == Qt::Key_P)
    {
        qDebug() << "Handling the P button - replot Torus";
        _scenario.replotTesttorus();
    }

    if( ke and ke->key() == Qt::Key_1)
    {
        qDebug() << "Pressing E - switch cam to Proj";
        _scenario.switchCam(1);
    }
    if( ke and ke->key() == Qt::Key_2)
    {
        qDebug() << "Pressing E - switch cam to Front";
        _scenario.switchCam(2);
    }
    if( ke and ke->key() == Qt::Key_3)
    {
        qDebug() << "Pressing E - switch cam to Side";
        _scenario.switchCam(3);
    }
    if( ke and ke->key() == Qt::Key_4)
    {
        qDebug() << "Pressing E - switch cam to Top";
        _scenario.switchCam(4);
    }

    if(ke and ke->key() == Qt::Key_R)
    {
        qDebug() << "Handling the R button - stop simulation";
        //_scenario.stopSimulation();
        _scenario.toggleSimulation();
    }

    _input_events.pop();
  }

}

void GuiApplication::handleMouseButtonPressedEvents(QMouseEvent *m)
{
    if( m->buttons() == Qt::LeftButton )
    {
        _startpos = _endpos;
        _endpos = {m->pos().x(),m->pos().y()};
        _leftMousePressed = true;
        qDebug() << "Left Mouse Button Pressed";
    }

    if( m->buttons() == Qt::RightButton )
    {
        _rightMousePressed = true;
        qDebug() << "Right Mouse Button Pressed";
    }



}

void GuiApplication::handleMouseMovementEvents(QMouseEvent *m)
{
    if(m->type()==QEvent::MouseMove && _leftMousePressed == true){
        _startpos = _endpos;
        _endpos = {m->pos().x(),m->pos().y()};
        _scenario.moveCamera(_startpos,_endpos);
   }
}

void GuiApplication::handleMouseButtonReleasedEvents(QMouseEvent *m)
{
    if( m->type() == QEvent::MouseButtonRelease )
    {
        _leftMousePressed = false;
        _rightMousePressed = false;
    }
}

void GuiApplication::handleWheelEvents(QWheelEvent *w)
{
    int delta = w->delta();

    if (w->modifiers() == Qt::NoModifier)
    {
        //_scenario.lockObject(true);
        if (delta<0){_scenario.zoomCamera(0.95);}
        if (delta>0) {_scenario.zoomCamera(1.05);}
    }

    if (w->modifiers() == Qt::ShiftModifier)
    {
        _scenario.panHorizontalCam(delta);
        //qDebug() << "shift incl";
    }

    if (w->modifiers() == Qt::ControlModifier)
    {
        _scenario.panVerticalCam(delta);
        //qDebug() << "ctrl incl";
    }


}



