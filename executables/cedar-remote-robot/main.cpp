/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        main.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 07 30

    Description: This executable starts a hardware robot that listens to remote commands and sends back sensor values.
                 Currently, only e-pucks are supported.

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/devices/YarpChannel.h"
#include "cedar/devices/Robot.h"
#include "cedar/auxiliaries/Path.h"
#include "cedar/devices/kteam/DriveSerial.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/CommandLineParser.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <QApplication>

std::string robotName;

void run()
{
  using cedar::dev::kteam::DriveSerialPtr;
  using cedar::dev::kteam::DriveSerial;
  using cedar::dev::Robot;
  typedef cedar::dev::YarpChannel<cv::Mat> YarpMatChannel;
  CEDAR_GENERATE_POINTER_TYPES(YarpMatChannel);

  // open a communication channel to receive and send motor values
  YarpMatChannelPtr yarp_channel(new YarpMatChannel());
  // open channels
  yarp_channel->addReaderPort(robotName + "/motorCommands");
  yarp_channel->addWriterPort(robotName + "/encoderValues");
  yarp_channel->open();
  
  // create a hardware robot that is able to execute the motor commands
  cedar::dev::RobotPtr robot(new Robot());
  cedar::aux::Path resource("resource://robots/epuck/serial_configuration.json");
  robot->readJson(resource.absolute().toString());
  std::cout << "This is robot " << robot->getName() << ", receiving motor commands and sending encoder values" << std::endl;

  // get the drive component, open the serial channel and start the hardware thread
  DriveSerialPtr drive_serial = boost::dynamic_pointer_cast<DriveSerial>(robot->getComponent("drive"));
  drive_serial->getChannel()->open();
  drive_serial->startCommunication();

  // in a loop, receive new motor commands vai yarp and read out encoder values, subsequently forwarded via yarp
  cv::Mat encoder_matrix(2,1,CV_32F);
  while (true)
  {
    // speeds
    std::vector<cedar::unit::Frequency> wheel_speeds;
    try
    {
      cv::Mat speeds = yarp_channel->read(robotName + "/motorCommands");
      if (speeds.rows == 2) // one value for each wheel
      {
        wheel_speeds.push_back(speeds.at<float>(0,0) * cedar::unit::hertz);
        wheel_speeds.push_back(speeds.at<float>(1,0)* cedar::unit::hertz);
        drive_serial->setWheelSpeedPulses(wheel_speeds);
      }
    }
    catch (cedar::aux::net::NetWaitingForWriterException& exc)
    {
      // do nothing here, writer is not up yet
    }

    // encoder
    std::vector<int> encoder_values = drive_serial->getEncoders();
    if (encoder_values.size() == 2)
    {
      encoder_matrix.at<float>(0,0) = encoder_values.at(0);
      encoder_matrix.at<float>(1,0) = encoder_values.at(1);
      yarp_channel->write(encoder_matrix, robotName + "/encoderValues");
    }
    // wait for 10 ms before next loop
    cedar::aux::sleep(cedar::unit::Time(10 * cedar::unit::milli * cedar::unit::seconds));
  }
  // not reachable, but just in case
  QApplication::exit(0);
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  cedar::aux::CommandLineParser parser;
  parser.setDescription
         (
           "This executable listens to remote e-puck commands and forwards them to an e-puck connected to this machine."
         );
  parser.defineValue("name", "The remote robot's name.", 'n');
  parser.parse(argc, argv, true);
  if (parser.hasParsedValue("name"))
  {
    robotName = parser.getValue<std::string>("name");
  }
  else
  {
    robotName = "new robot";
  }

  cedar::aux::CallFunctionInThread caller(boost::bind(&run));

  caller.start();

  app.exec();
  
  return 0;
}
#endif // YARP?
