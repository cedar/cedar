/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KukaInterface.cpp

 ----- Author:      Guido Knips

 ----- Email:       guido.knips@ini.rub.de

 ----- Date:        2010 11 23

 ----- Description: cedar-Interface for the KUKA LBR

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/
//Ready for some KukaIn?


#ifndef CEDAR_DEV_ROBOT_KUKA_KUKA_INTERFACE_H
#define CEDAR_DEV_ROBOT_KUKA_KUKA_INTERFACE_H

// LOCAL INCLUDES (includes from this project)
// none yet

// PROJECT INCLUDES
#include "../KinematicChain.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"
#include "cedar/namespace.h"
// SYSTEM INCLUDES
#include <QObject>
#include <fri/friremote.h>


/*!@brief cedar Interface for the KUKA LBR
 * This class wraps the KUKA Fast Research Interface (FRI)
 */
class cedar::dev::robot::kuka::KukaInterface :  public cedar::dev::robot::KinematicChain,
                                                public cedar::aux::ConfigurationInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
private:
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Constructor that takes the name of the configuration file to use with the object.
   * @param configFileName    Name of the configuration file containing the parameters
   */
  KukaInterface(const std::string& configFileName);

  /*!the Destructor*/
  virtual ~KukaInterface();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief returns angle for a specified joint
   *  @param index  index of the joint, since the KUKA LBR has seven of them, it must be in the interval [0,6]
   *  @return joint angle for the given index
   */
  virtual const double getJointAngle(const unsigned int index) const;
  /*! @brief returns all joint angles
   *  @return a vector filled with the joint angles
   */
  virtual const std::vector<double> getJointAngles() const;
  /*! @brief returns all joint angles
   *  @return a OpenCV-Matrix filled with the joint angles
   */
  virtual const cv::Mat getJointAnglesMatrix() const;
  /*! @brief set the angle for a specified joint
   *  @param index  index of the joint
   *  @param angle  angle to be set, in radian measure
   */
  virtual void setJointAngle(const unsigned int index, const double angle);
  /*! @brief set the angle for all joints
   *  @param angles vector of angles to be set, in radian measure
   */
  virtual void setJointAngles(const std::vector<double>& angles);
  /*! @brief set the angle for all joints
   *  @param angles OpenCV-Matrix of angles to be set, in radian measure
   */
  virtual void setJointAngles(const cv::Mat& angleMatrix);
  //--------------------------------------------------------------------------------------------------------------------
  // public slots
  //--------------------------------------------------------------------------------------------------------------------
public slots:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief This method initializes the object.
   * This method is called from all constructors of the class.
   */
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  //true, if the object has ben initialized
  bool mIsInit;
  //KUKA Vendor-Interface, wrapped by this class
  friRemote *mpFriRemote;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
protected:
  // none yet
private:
  //!IP Address of the remote host
  std::string _mRemoteHost;
  //!local server port
  int _mServerPort;


};

#endif /* CEDAR_DEV_ROBOT_KUKA_KUKA_INTERFACE_H */
