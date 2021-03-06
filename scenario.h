#ifndef SCENARIO_H
#define SCENARIO_H

// local
class TestTorus;

// gmlib
namespace GMlib {

  class Scene;
  class Camera;
  class PointLight;
  class DefaultRenderer;
  class RenderTarget;
  class DefaultSelectRenderer;
  class SceneObject;

  template <typename T, int n>
  class Point;

  template <typename T>
  class PTorus;

  template <typename T, int n>
  class Vector;

  template <typename T>
  class PSphere;

  template <typename T>
  class PCylinder;

  template <typename T>
  class PPlane;


  class Angle;
}

// qt
#include <QObject>
#include <QRect>
#include <QDebug>

// stl
#include <iostream>
#include <memory>
#include <queue>

//struct ViewPair
//{
//    std::shared_ptr<GMlib::DefaultRenderer>     renderer { nullptr };
//    std::shared_ptr<GMlib::Camera>              camera   { nullptr };
//};


class Scenario: public QObject {
  Q_OBJECT
public:
  explicit Scenario();
  virtual ~Scenario();

  void                                              initialize();
  void                                              deinitialize();
  virtual void                                      initializeScenario();

  void                                              startSimulation();
  void                                              stopSimulation();
  void                                              toggleSimulation();
  bool                                              isSimulationRunning();

  void                                              render( const QRect& viewport, GMlib::RenderTarget& target );
  void                                              prepare();

  void                                              replotLow();
  void                                              replotHigh();



  GMlib::Point<int, 2>                              fromQtToGMlibViewPoint(const QPoint& pos);

  void                                              moveCamera(const QPoint& begin_pos, const QPoint& end_pos);
  float                                             getScale();
  void                                              zoomCamera(const float &zoom_val);
  void                                              panHorizontalCam(int wheel_delta);
  void                                              panVerticalCam(int wheel_delta);
  void                                              switchCam(int n);

  void                                              lockToObject(const QPoint& qpos);


  GMlib::SceneObject*                               findSceneObj(const QPoint& qpos);

  void                                              selectObject(const QPoint& qpos);
  void                                              selectObjects(const QPoint& qpos);

  void                                              selectSubObjects(GMlib::SceneObject* object);
  void                                              selectAllObjects();

  void                                              toogleSelectionAllObjects();
  void                                              deselectAllObjects();

  void                                              camFly(char direction);
  void                                              save();
  void                                              load();
  void                                              moveObj(const QPoint& begin_pos, const QPoint& end_pos);
  void                                              rotateObj(const QPoint& begin_pos, const QPoint& end_pos);
  void                                              scaleObj(int &delta);
  void                                              changeColor();
  void                                              resetCam(); // unlock camera, return camera position to default
  void                                              insertObject(char type, const QPoint& pos);
  void                                              insertSphere(float radius, const QPoint& pos);
  void                                              deleteObject();

  void                                              testsPoint(const QPoint& pos);

  std::queue<std::shared_ptr<GMlib::SceneObject>>   _sceneObjectQueue;

protected:
  void                                              timerEvent(QTimerEvent *e) override;

private:
  std::shared_ptr<GMlib::Scene>                     _scene;
  int                                               _timer_id;

  std::shared_ptr<GMlib::DefaultSelectRenderer>     _select_renderer;
  std::shared_ptr<GMlib::DefaultRenderer>           _renderer { nullptr }; //active renderer
  std::shared_ptr<GMlib::Camera>                    _camera   { nullptr }; //active cam



  //std::vector<std::shared_ptr<GMlib::Camera>>             _camerasVec;
  //std::vector<std::shared_ptr<GMlib::DefaultRenderer>>    _rendererVec;

  //std::vector<std::shared_ptr<ViewPair>>                  _camrendViews {nullptr}; //IDK why it crashes when using smart pointer with struct
  //std::pair<GMlib::DefaultRenderer, GMlib::Camera>        _pair;

  //std::vector<int> _i;

  QRect                                             _viewport { QRect(0,0,1,1) };

  std::shared_ptr<GMlib::PointLight>                _light;
  std::shared_ptr<TestTorus>                        _testtorus;

  //std::shared_ptr<GMlib::Vector<GMlib::SceneObject>>    _selectedObjects;

private:
  void                                              save( std::ofstream& os, const GMlib::SceneObject* obj);
  void                                              saveSO( std::ofstream& os, const GMlib::SceneObject* obj);
  void                                              savePT( std::ofstream& os, const GMlib::PTorus<float>* obj);
  void                                              savePS( std::ofstream &os, const GMlib::PSphere<float> *obj);
  void                                              savePC( std::ofstream &os, const GMlib::PCylinder<float> *obj);
  void                                              savePP( std::ofstream &os, const GMlib::PPlane<float> *obj);

private:
  static std::unique_ptr<Scenario>                  _instance;
public:
  static Scenario&                                  instance();


public slots:
  void clicksave()
  {
      this->save();
  }

  void clickload()
  {
      this->load();
  }
};

#endif
