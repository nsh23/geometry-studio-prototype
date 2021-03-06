#include "guiapplication.h"

// qt
#include <QOpenGLContext>
#include <QDebug>

#include <QQmlContext>
#include <QQuickItem>
#include <QStringListModel>

// stl
#include <iostream>
#include <cassert>


GuiApplication::GuiApplication(int& argc, char **argv) : QGuiApplication(argc, argv) {

  setApplicationDisplayName( "Hello GMlib app display name," );
  //setApplicationDisplayName( "3DS Studio v2" );

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

  connect( &_window, &Window::signKeyReleased,
           this,     &GuiApplication::handleKeyRelease );

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


  _window.rootContext()->setContextProperty( "scenario", &_scenario);
  //_window.rootContext()->setContextProperty( "save", _scenario.save());
  //_window.rootContext()->setContextProperty( "load", _scenario.load());

  _window.setSource(QUrl("qrc:///qml/main.qml"));
  _window.show();
 // _window.setTitle(" ");


//button stance initializing
  _controlPressed = false;
  _shiftPressed = false;
  _altPressed = false;
  _leftMousePressed = false;
  _rightMousePressed = false;
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

    if(e->key() == Qt::Key_Q && e->modifiers() == Qt::ControlModifier)
    {
        _window.close();
    }

//    else if(e->key() == Qt::Key_S)
//    {
//        _scenario.save();
//        //qDebug() << "File saved";
//    }

//    else if(e->key() == Qt::Key_L)
//    {
//        _scenario.load();
//        //qDebug() << "File loaded";
//    }

    //cam fly controls

    else if (e->key() == Qt::Key_U)
    {

        _scenario.resetCam();
        qDebug() << "Reseting cam";
    }


    else if (e->key() == Qt::Key_Up)
    {
        _scenario.camFly('F');
    }

    else if (e->key() == Qt::Key_Down)
    {
        _scenario.camFly('B');
    }

    else if (e->key() == Qt::Key_Right)
    {
        _scenario.camFly('R');
    }

    else if (e->key() == Qt::Key_Left)
    {
        _scenario.camFly('L');
    }

    //end cam fly controls

    else if(e->modifiers() == Qt::ShiftModifier)
    {
        _shiftPressed = true;
        qDebug() << "Shift pressed";
    }

    else if(e->modifiers() == Qt::ControlModifier)
    {
        _controlPressed = true;
        qDebug() << "CTRL pressed";
    }

    else if(e->modifiers() == Qt::AltModifier)
    {
        _altPressed = true;
        qDebug() << "Alt pressed";
    }

    else
    {
        _input_events.push(std::make_shared<QKeyEvent>(*e));
    }

}

void GuiApplication::handleKeyRelease (QKeyEvent* e)
{
    if (e->type()==QEvent::KeyRelease && _controlPressed == true)
    {
        _controlPressed = false;
        qDebug() << "CTRL released";

    }
    if (e->type()==QEvent::KeyRelease && _shiftPressed == true)
    {
        _shiftPressed = false;
        qDebug() << "Shift released";
    }
    if (e->type()==QEvent::KeyRelease && _altPressed == true)
    {
        _altPressed = false;
        qDebug() << "Alt released";
    }
}

void GuiApplication::handleGLInputEvents() { //for OpenGL methods

  while(!_input_events.empty())
  {
    const auto& e  = _input_events.front();
    const auto& ke = std::dynamic_pointer_cast<const QKeyEvent>(e);
    const auto& we = std::dynamic_pointer_cast<const QWheelEvent>(e); //wheel events
    const auto& me = std::dynamic_pointer_cast<const QMouseEvent>(e); //for mouse events

    if(we)
    {
        if(we->modifiers()==Qt::AltModifier) //and selectedobjects not empty)
        {
            int delta = we->delta();
            _scenario.scaleObj(delta);
        }
    }

    if(ke and ke->key() == Qt::Key_N)
    {
        qDebug() << "Handling the N button - new sphere";
        _scenario.insertSphere(1, _endpos);
    }

    if(ke and ke->key() == Qt::Key_D)
    {
        qDebug() << "Deleting selected objects";
        _scenario.deleteObject();
    }

    if(ke and ke->key() == Qt::Key_P)
    {
        qDebug() << "Handling the P button - replot low";
        _scenario.replotLow();
    }

    if(ke and ke->key() == Qt::Key_O)
    {
        qDebug() << "Handling the O button - replot high";
        _scenario.replotHigh();
    }

    if(ke and ke->key() == Qt::Key_A)
    {
        qDebug() << "A - select or deselect all objects";
        _scenario.toogleSelectionAllObjects();
    }

    if(ke and ke->key()==Qt::Key_C)
    {
        _scenario.changeColor();
    }

    if( ke and ke->key() == Qt::Key_1)
    {
        qDebug() << "Pressing 1 - switch cam to Proj";
        _scenario.switchCam(1);
    }
    if( ke and ke->key() == Qt::Key_2)
    {
        qDebug() << "Pressing 2 - switch cam to Front";
        _scenario.switchCam(2);
    }
    if( ke and ke->key() == Qt::Key_3)
    {
        qDebug() << "Pressing 3 - switch cam to Side";
        _scenario.switchCam(3);
    }
    if( ke and ke->key() == Qt::Key_4)
    {
        qDebug() << "Pressing 4 - switch cam to Top";
        _scenario.switchCam(4);
    }

    if(ke and ke->key() == Qt::Key_R)
    {
        qDebug() << "Handling the R button - toggle simulation";
        _scenario.toggleSimulation();
    }

    if ( (me and me->buttons() == Qt::RightButton) && me->modifiers() == Qt::NoModifier)
    {
        qDebug() << "Handling RMB - select object";
        _scenario.selectObject(_endpos);
    }

    if ((me and me->buttons() == Qt::RightButton) && me->modifiers() == Qt::ShiftModifier)
    {
        qDebug() << "Handling RMB with control - multiple objects selection";
        _scenario.selectObjects(_endpos);
    }

    if ((me and me->buttons() == Qt::RightButton) && me->modifiers() == Qt::ControlModifier)
    {
        _scenario.lockToObject(_endpos);
    }

    if(me and me->buttons()==Qt::LeftButton)
    {
        if(me->modifiers()==Qt::ShiftModifier)
        {
            _scenario.moveObj(_startpos,_endpos);
        }
        if(me->modifiers()==Qt::ControlModifier)
        {
            _scenario.rotateObj(_startpos,_endpos);
        }

        if(me and me->type()==QEvent::MouseMove)
        {
            if(me->modifiers()==Qt::ShiftModifier)
            {
                _scenario.moveObj(_startpos,_endpos);
            }

            if(me->modifiers()==Qt::ControlModifier)
            {
                _scenario.rotateObj(_startpos,_endpos);
            }
        }
    }

    _input_events.pop();

  }
}

void GuiApplication::handleMouseButtonPressedEvents(QMouseEvent *m)
{
    //update position
    _startpos = _endpos;
    _endpos.setX(m->pos().x());
    _endpos.setY(m->pos().y());

    if( m->buttons() == Qt::LeftButton )
    {
        //qDebug() << "Left Mouse Button Pressed";
        _leftMousePressed = true;


        //qDebug() << "QPoint";
        qDebug() << _endpos;
        _scenario.testsPoint(_endpos);

        //qDebug() << "startpos";
        //qDebug() << _startpos;
    }

    if( m->buttons() == Qt::RightButton )
    {
        //qDebug() << "Right Mouse Button Pressed";
        _rightMousePressed = true;
    }

    _input_events.push(std::make_shared<QMouseEvent>(*m));
}

void GuiApplication::handleMouseMovementEvents(QMouseEvent *m)
{
    if(m->type()==QEvent::MouseMove && _leftMousePressed == true)
    {
        _startpos = _endpos;
        _endpos.setX(m->pos().x());
        _endpos.setY(m->pos().y());

        if (m->modifiers() != Qt::NoModifier)
        {
            _input_events.push(std::make_shared<QMouseEvent>(*m));
        }

        if (m->modifiers() == Qt::NoModifier)
        {
            _scenario.moveCamera(_startpos,_endpos);
        }
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
    //if (!w->modifiers())
    {
        //_scenario.lockObject(true);
        if (delta<0)
        {
            _scenario.zoomCamera(0.95);
        }
        if (delta>0)
        {
            _scenario.zoomCamera(1.05);
        }
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

    if (w->modifiers() == Qt::AltModifier)
    {
        _input_events.push(std::make_shared<QWheelEvent>(*w));
    }
}



