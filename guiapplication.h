#ifndef GUIAPPLICATION_H
#define GUIAPPLICATION_H

#include "scenario.h"
#include "window.h"

// qt
#include <QGuiApplication>

// stl
#include <memory>
#include <queue>




class GuiApplication : public QGuiApplication {
  Q_OBJECT
public:
  explicit GuiApplication(int& argc, char** argv);
  ~GuiApplication();

private:
  Window                                      _window;
  Scenario                                    _scenario;

  bool                                        _leftMousePressed;
  bool                                        _rightMousePressed;

  bool                                        _controlPressed;
  bool                                        _shiftPressed;
  bool                                        _altPressed;

  QPoint                                      _startpos;
  QPoint                                      _endpos;

  QPoint                                      _qpos;

  std::queue<std::shared_ptr<QInputEvent>>    _input_events;

  

private slots:

  virtual void                                onSceneGraphInitialized();
  virtual void                                afterOnSceneGraphInitialized();

  virtual void                                onSceneGraphInvalidated();

  void                                        handleMouseButtonPressedEvents(QMouseEvent*);
  void                                        handleMouseMovementEvents(QMouseEvent*);
  void                                        handleMouseButtonReleasedEvents(QMouseEvent*);
  void                                        handleWheelEvents(QWheelEvent*);

  void                                        handleKeyPress( QKeyEvent* );
  void                                        handleKeyRelease( QKeyEvent* );

  void                                        handleGLInputEvents();


signals:
  void                                        signOnSceneGraphInitializedDone();
};

#endif // GUIAPPLICATION_H
