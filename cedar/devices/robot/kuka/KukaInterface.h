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
#include "namespace.h"
#include "KukaCommunicator.h"

// PROJECT INCLUDES
#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"

// SYSTEM INCLUDES
#include <fri/friremote.h>


/*!@brief cedar Interface for the KUKA LBR

 * This class wraps the KUKA Fast Research Interface (FRI)
 */
class cedar::dev::robot::kuka::KukaInterface :  public cedar::dev::robot::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Constructor that takes the name of the configuration file to use with the object.

   * @param configFileName    Name of the configuration file containing the parameters
   * @param communicatorConfigFileName  Name of the configuration file for the communicator member variable
   */
  KukaInterface(const std::string& configFileName, const std::string& communicatorConfigFileName);

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
  virtual double getJointAngle(const unsigned int index) const;
  /*! @brief returns all joint angles

   *  @return a vector filled with the joint angles
   *  \throws cedar::aux::exc::IndexOutOfRangeException if index is bigger than allowed
   */
  virtual std::vector<double> getJointAngles() const;
  /*! @brief returns all joint angles

   *  @return a OpenCV-Matrix filled with the joint angles
   */
  virtual cv::Mat getJointAnglesMatrix() const;
  /*! @brief set the angle for a specified joint

   *  @param index  index of the joint
   *  @param angle  angle to be set, in radian measure
   *  \throws cedar::aux::exc::BadConnectionException if the robot is not in command mode
   */
  virtual void setJointAngle(const unsigned int index, const double angle);
  /*! @brief set the angle for all joints

   *  @param angles vector of angles to be set, in radian measure
   *  \throws cedar::aux::exc::BadConnectionException if the robot is not in command mode
   *  \throws cedar::aux::exc::IndexOutOfRangeException if index is bigger than allowed
   */
  virtual void setJointAngles(const std::vector<double>& angles);
  /*! @brief set the angle for all joints

   *  @param angles OpenCV-Matrix of angles to be set, in radian measure
   *  \throws cedar::aux::exc::BadConnectionException if the robot is not in command mode
   */
  virtual void setJointAngles(const cv::Mat& angleMatrix);

  /*Wrapping of some FRI-Functions that are needed for ensuring connection quality*/

  /*! @brief returns the state of the Interface.

   * this can be FRI_STATE_OFF, FRI_STATE_MON and FRI_STATE_CMD
   * Commands can only be send if the state is FRI_STATE_CMD, which represents the command mode
   * @return current state of the interface
   */
  FRI_STATE getFriState()const;
  /*! @brief returns the quality of the connection.

   * this can range from FRI_QUALITY_UNACCEPTABLE to FRI_QUALITY_PERFECT
   * if the Quality is worse (means: less) than FRI_QUALITY_GOOD, command mode switches to monitor mode automatically
   * @return current Quality of the connection
   */
  FRI_QUALITY getFriQuality()const;
  /*! @brief does Data exchange with the KUKA-LBR

   * normally not necessary, the functions setJointAngle() and setJointAngles() do this by themselves.
   * the get-Functions don't, though
   */
  float getSampleTime()const;
  /* @brief check if the robot is powered

   * This method does not call doDataExchange itself
   * this especially means the dead man switch is in the right position and the robot is in command mode
   * @return true, if power is on
   */
  bool isPowerOn()const;

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
   *
   * This method is called from all constructors of the class.
   * @param commandMode establish command mode if true
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
  //KukaCommunicator, a looped thread that communicates steadily with the KUKA-RC and allows to send commands
  cedar::dev::robot::kuka::KukaCommunicator *mpCommunicator;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
protected:
  // none yet
private:
  //none yet
};

#endif /* CEDAR_DEV_ROBOT_KUKA_KUKA_INTERFACE_H */
