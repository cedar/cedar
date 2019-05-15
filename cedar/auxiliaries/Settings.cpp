/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Settings.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 06 21

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/FFTWPlanningStrategy.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/SetParameter.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/PluginProxy.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/property_tree/json_parser.hpp>
#endif
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <QApplication>

#ifdef CEDAR_USE_YARP
#include <yarp/os/Network.h>
#include <yarp/os/Contact.h>
#include <yarp/conf/version.h>
#endif

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Settings::Settings()
:
cedar::aux::Configurable(),
mGlobalTimeFactor(1.0),
_mMemoryDebugOutput(new cedar::aux::BoolParameter(this, "memory debug output", false)),
_mRecorderSerializationFormat(new cedar::aux::EnumParameter(this, "recorder data format", cedar::aux::SerializationFormat::typePtr(), cedar::aux::SerializationFormat::CSV)),
_mYarpConfigInfo(new cedar::aux::StringParameter(this,"yarp config info","134.147.176.97 10000"))
{
  _mRecorderWorkspace = new cedar::aux::DirectoryParameter
                        (
                          this,
                          "recorder output directory",
                          cedar::aux::getUserHomeDirectory() + "/cedarRecordings/"
                        );

  cedar::aux::ConfigurablePtr plugins(new cedar::aux::Configurable());
  this->addConfigurableChild("plugins", plugins);

  this->_mPluginsToLoad = new cedar::aux::StringSetParameter
                          (
                            plugins.get(),
                            "load",
                            (std::set<std::string>())
                          );

  this->_mKnownPlugins = new cedar::aux::StringSetParameter
                         (
                           plugins.get(),
                           "list",
                           (std::set<std::string>())
                         );

  this->_mPluginSearchPaths = new cedar::aux::StringVectorParameter
                              (
                                plugins.get(),
                                "search paths",
                                (std::vector<std::string>())
                              );

#ifdef CEDAR_USE_FFTW
  this->_mFFTWNumberOfThreads = new cedar::aux::UIntParameter
                                (
                                  this,
                                  "number of FFTW threads",
                                  1
                                );

  this->_mFFTWPlanningStrategy = new cedar::aux::EnumParameter
                                 (
                                   this,
                                   "FFTW planning strategy",
                                   cedar::aux::conv::FFTWPlanningStrategy::typePtr(),
                                   cedar::aux::conv::FFTWPlanningStrategy::Estimate
                                 );
#endif // CEDAR_USE_FFTW

  try
  {
    this->load();
  }
  catch (cedar::aux::ParseException& exc)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Error loading auxiliary settings, a new file will be generated.",
      "cedar::aux::Settings::Settings()"
    );
  }

  this->_mYarpConfigInfo->setConstant(true);
#ifdef CEDAR_USE_YARP
#if (YARP_VERSION_MAJOR > 2)
  this->_mYarpConfigInfo->setConstant(false);
#endif
#endif

#ifdef CEDAR_PORTABLE
#ifndef CEDAR_PORTABLE_MAC_BUNDLE_PATHS
  boost::filesystem::create_directories(boost::filesystem::current_path().string() + "/../plugins/");
#endif
  boost::filesystem::directory_iterator end_iter;
  std::vector<cedar::aux::Path> folders;
#ifdef CEDAR_PORTABLE_MAC_BUNDLE_PATHS
  for (boost::filesystem::directory_iterator dir_iter(QCoreApplication::applicationDirPath().toStdString() + "/../Cedar_Plugins/"); dir_iter != end_iter ; ++dir_iter)
#else
  for (boost::filesystem::directory_iterator dir_iter(boost::filesystem::current_path().string() + "/../plugins/"); dir_iter != end_iter ; ++dir_iter)
#endif
  {
    if (boost::filesystem::is_directory(dir_iter->status()))
    {
      std::string folder = cedar::aux::toString(*dir_iter);
      if (folder.size() > 0 && folder.at(0) == '"')
      {
        folder = folder.substr(1);
      }
      if (folder.size() > 0 && folder.at(folder.size() - 1) == '"')
      {
        folder = folder.substr(0, folder.size() - 1);
      }
#ifdef CEDAR_PORTABLE_MAC_BUNDLE_PATHS
      QString fpath = QString::fromStdString(folder).remove(QCoreApplication::applicationDirPath());
      std::string name = QString::fromStdString(fpath.toStdString()).remove("/../Cedar_Plugins/").toStdString();
      std::string search_path = "." + fpath.toStdString();
#else
      QString fpath = QString::fromStdString(folder).remove(QString::fromStdString(boost::filesystem::current_path().string()));
      std::string name = QString::fromStdString(fpath.toStdString()).remove("/../plugins/").toStdString();
      std::string search_path = "." + fpath.toStdString();
#endif
      if (!this->_mPluginSearchPaths->contains(search_path))
      {
        this->_mPluginSearchPaths->pushBack(search_path);
      }
      if (this->_mKnownPlugins->get().find(name) == this->_mKnownPlugins->get().end())
      {
        this->_mKnownPlugins->insert(name);
      }
    }
  }
#endif // CEDAR_PORTABLE
}

cedar::aux::Settings::~Settings()
{
  try
  {
    this->save();
  }
  catch(cedar::aux::ParseException& exc)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error saving settings, please check file permissions in"
        + cedar::aux::getUserApplicationDataDirectory() + "/.cedar",
      "cedar::aux::Settings::~Settings()"
    );
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::SerializationFormat::Id cedar::aux::Settings::getSerializationFormat() const
{
  QReadLocker l(this->_mRecorderSerializationFormat->getLock());
  auto copy = this->_mRecorderSerializationFormat->getValue();
  l.unlock();
  return copy;
}

void cedar::aux::Settings::setSerializationFormat(cedar::aux::SerializationFormat::Id format)
{
  this->_mRecorderSerializationFormat->setValue(format, true);
}

cedar::aux::EnumParameterPtr cedar::aux::Settings::getRecorderSerializationFormatParameter() const
{
  return this->_mRecorderSerializationFormat;
}

void cedar::aux::Settings::setGlobalTimeFactor(double factor)
{
  QWriteLocker locker(this->mGlobalTimeFactor.getLockPtr());
  this->mGlobalTimeFactor.member() = factor;
  locker.unlock();

  // note, that we do not pass this->mGlobalTimeFactor here to prevent race conditions
  this->signalGlobalTimeFactorChanged(factor);
}

double cedar::aux::Settings::getGlobalTimeFactor() const
{
  QReadLocker locker(this->mGlobalTimeFactor.getLockPtr());
  double copy = this->mGlobalTimeFactor.member();
  locker.unlock();
  return copy;
}

cedar::aux::DirectoryParameterPtr cedar::aux::Settings::getRecorderWorkspaceParameter() const
{
  return this->_mRecorderWorkspace;
}

cedar::aux::StringParameterPtr cedar::aux::Settings::getYarpConfigInfoParameter() const
{
  return this->_mYarpConfigInfo;
}

std::string cedar::aux::Settings::getRecorderOutputDirectory() const
{
  return this->_mRecorderWorkspace->getValue().absolutePath().toStdString();
}

void cedar::aux::Settings::swapPluginSearchPaths(unsigned int first, unsigned int second)
{
  this->_mPluginSearchPaths->swap(first, second);

  this->mSearchPathsSwappedSignal(first, second);
  this->mSearchPathsChangedSignal();
}

const std::set<std::string>& cedar::aux::Settings::getKnownPlugins() const
{
  return this->_mKnownPlugins->get();
}

void cedar::aux::Settings::addPlugin(cedar::aux::PluginProxyPtr plugin)
{
  std::string search_path = plugin->getNormalizedSearchPath();

  if (!this->_mPluginSearchPaths->contains(search_path))
  {
    cedar::aux::LogSingleton::getInstance()->message
    (
      "Plugin \"" + plugin->getPluginName() + "\" was not found in the current search paths. Adding its path.",
      "cedar::aux::Settings::addPlugin(cedar::aux::PluginProxyPtr)"
    );
    this->addPluginSearchPath(search_path);
  }

  const std::string& name = plugin->getPluginName();
  if (this->_mKnownPlugins->get().find(name) == this->_mKnownPlugins->get().end())
  {
    this->_mKnownPlugins->insert(plugin->getPluginName());
    this->mPluginAddedSignal(plugin->getPluginName());
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->message
    (
      "Plugin \"" + plugin->getPluginName() + "\" is already known.",
      "cedar::aux::Settings::addPlugin(cedar::aux::PluginProxyPtr)"
    );
  }
}

void cedar::aux::Settings::removePlugin(const std::string& pluginName)
{
  auto iter = this->_mKnownPlugins->get().find(pluginName);
  if (iter == this->_mKnownPlugins->get().end())
  {
    CEDAR_THROW(cedar::aux::UnknownPluginException, "The plugin \"" + pluginName + "\" is not known.");
  }

  auto load_iter = this->_mPluginsToLoad->get().find(pluginName);
  if (load_iter != this->_mPluginsToLoad->get().end())
  {
    this->_mPluginsToLoad->get().erase(load_iter);
  }

  this->_mKnownPlugins->get().erase(iter);
  this->mPluginRemovedSignal(pluginName);
}

bool cedar::aux::Settings::isPluginLoadedOnStartup(const std::string& pluginName) const
{
  return this->_mPluginsToLoad->contains(pluginName);
}


void cedar::aux::Settings::loadDefaultPlugins()
{
  const std::set<std::string>& plugins = this->pluginsToLoad();
  for (std::set<std::string>::const_iterator iter = plugins.begin(); iter != plugins.end(); ++ iter)
  {
    std::string action = "reading";
    try
    {
      action = "opening";
      const std::string& plugin_name = *iter;
      cedar::aux::PluginProxyPtr plugin = cedar::aux::PluginProxy::getPlugin(plugin_name);
      action = "loading";
      plugin->declare();
      cedar::aux::LogSingleton::getInstance()->systemInfo
      (
        "Loaded default plugin \"" + (*iter) + "\"",
        "void cedar::proc::Manager::loadDefaultPlugins()"
      );
    }
    catch (const cedar::aux::ExceptionBase& e)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Error while " + action + " default plugin \"" + (*iter) + "\": " + e.exceptionInfo(),
        "void cedar::proc::Manager::loadDefaultPlugins()"
      );
    }
    catch (std::exception& e)
    {
      std::string what = e.what();
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Error while " + action + " default plugin \"" + (*iter) + "\": " + what,
        "void cedar::proc::Manager::loadDefaultPlugins()"
      );
    }
    catch (...)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Unknown error while " + action + " default plugin.",
        "void cedar::proc::Manager::loadDefaultPlugins()"
      );
    }
  }
}

void cedar::aux::Settings::addPluginSearchPath(const std::string& path)
{
  this->_mPluginSearchPaths->pushBack(path);
  mPathAddedSignal(path);
  mSearchPathsChangedSignal();
}

void cedar::aux::Settings::removePluginSearchPath(const std::string& path)
{
  this->_mPluginSearchPaths->eraseAll(path);
  this->mSearchPathRemovedSignal(path);
  mSearchPathsChangedSignal();
}

void cedar::aux::Settings::removePluginSearchPath(size_t index)
{
  this->_mPluginSearchPaths->eraseIndex(index);
  mSearchPathIndexRemovedSignal(index);
  mSearchPathsChangedSignal();
}

const std::vector<std::string>& cedar::aux::Settings::getPluginSearchPaths() const
{
  return this->_mPluginSearchPaths->getValue();
}

void cedar::aux::Settings::load()
{
  std::string basepath = cedar::aux::getUserApplicationDataDirectory();
//  std::string path = cedar::aux::getUserApplicationDataDirectory() + "/.cedar/auxilariesSettings";
  try
  {
    this->readJson(basepath + "/.cedar/auxiliariesSettings");
    return;
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not read settings: " + std::string(e.what()) + "; Trying a different name.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }

  // backwards compatibility: there used to be a typo in the name of the settings file
  try
  {
    this->readJson(basepath + "/.cedar/auxilariesSettings");
    return;
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    CEDAR_THROW(cedar::aux::ParseException, "Error reading settings: " + std::string(e.what()));
  }
}

void cedar::aux::Settings::save()
{
  std::string path = cedar::aux::getUserApplicationDataDirectory() + "/.cedar/auxiliariesSettings";
  try
  {
    this->writeJson(path);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    CEDAR_THROW(cedar::aux::ParseException, "Error saving settings: " + std::string(e.what()));
  }


}


void cedar::aux::Settings::updateYarpNameServerContact()
{
#ifdef CEDAR_USE_YARP

#if (YARP_VERSION_MAJOR > 2)

  //Extract IP And Portnumber from Config String
  std::string configInfoString = this->getYarpConfigInfoParameter()->getValue();
  std::vector<std::string> splitStrings;
  boost::split(splitStrings, configInfoString, boost::is_any_of(" "));
  std::string hostName = splitStrings.at(0);
  int portNumber = boost::lexical_cast<int>(splitStrings.at(1));

  yarp::os::Contact contact = yarp::os::Network::getNameServerContact();
//  std::cout<<"Old Contact.Host: " << contact.getHost()  <<" Name: " << contact.getName()<<" RegName: "<< contact.getRegName() << " Carrier: " << contact.getCarrier()  <<std::endl;

  yarp::os::Contact contactNew(contact); //Copy Constructor
  contactNew.setHost(hostName);
  contactNew.setPort(portNumber);
//  contactNew.setName("/root"); //Default should be root already

   yarp::os::Network::setNameServerContact(contactNew);
//  std::cout<<"Changed Contact: "<< yarp::os::Network::getNameServerContact().getHost()<< std::endl;

  //Somehow reset Network Connections ? For now architecture Reset is neccessary
#endif
#endif
}

bool cedar::aux::Settings::getMemoryDebugOutput() const
{
  return this->_mMemoryDebugOutput->getValue();
}

const std::set<std::string>& cedar::aux::Settings::pluginsToLoad()
{
  return this->_mPluginsToLoad->get();
}

void cedar::aux::Settings::addPluginToLoad(const std::string& path)
{
  this->_mPluginsToLoad->insert(path);
}

void cedar::aux::Settings::removePluginToLoad(const std::string& path)
{
  std::set<std::string>::iterator pos = this->_mPluginsToLoad->get().find(path);
  if (pos != this->_mPluginsToLoad->get().end())
  {
    this->_mPluginsToLoad->get().erase(pos);
  }
}

std::string cedar::aux::Settings::getCurrentArchitectureFileName()
{
  return mCurrentArchitectureFileName;
}

void cedar::aux::Settings::setCurrentArchitectureFileName(std::string newFileName)
{
  mCurrentArchitectureFileName = newFileName;

  emit currentArchitectureFileChanged();
}

#ifdef CEDAR_USE_FFTW
unsigned int cedar::aux::Settings::getFFTWNumberOfThreads() const
{
  return this->_mFFTWNumberOfThreads->getValue();
}

cedar::aux::EnumId cedar::aux::Settings::getFFTWPlanningStrategy() const
{
  return this->_mFFTWPlanningStrategy->getValue().id();
}

std::string cedar::aux::Settings::getFFTWPlanningStrategyString() const
{
  return this->_mFFTWPlanningStrategy->getValue().name();
}
#endif // CEDAR_USE_FFTW
