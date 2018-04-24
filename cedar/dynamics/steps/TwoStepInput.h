/*
 * TwoStepInput.h
 *
 *  Created on: Apr 22, 2014
 *      Author: tekuejpp
 */

#ifndef TWOSTEPINPUT_H_
#define TWOSTEPINPUT_H_



// CEDAR INCLUDES
#include "cedar/dynamics/steps/TwoStepInput.fwd.h"
#include "cedar/dynamics/Dynamics.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"


// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"

// SYSTEM INCLUDES


/*!@brief Generates a matrix with a Gaussian.
 *
 *        The output matrix will contain values of a Gauss function, sampled based on the indices of the matrix taken as
 *        x,y,... coordinates.
 */
class cedar::dyn::steps::TwoStepInput : public cedar::dyn::Dynamics
{
	//--------------------------------------------------------------------------------------------------------------------
	// macros
	//--------------------------------------------------------------------------------------------------------------------
	Q_OBJECT
	//--------------------------------------------------------------------------------------------------------------------
	// constructors and destructor
	//--------------------------------------------------------------------------------------------------------------------
public:
	//!@brief The standard constructor.
	TwoStepInput();

	//!@brief Destructor

	//--------------------------------------------------------------------------------------------------------------------
	// public methods
	//--------------------------------------------------------------------------------------------------------------------
public:
	//!@brief Sets the center of the Gauss in the given dimension.
	void setCenter(unsigned int dimension, double center);

	void setSecondCenter(unsigned int dimension, double center);


	double getCenter(unsigned int dimension);

	double getSecondCenter(unsigned int dimension);

	cv::Mat getCurrentTarget();
	//!@brief Sets the amplitude of the Gauss input.
	void setAmplitude(double amplitude);

	//!@brief Returns the amplitude of the Gauss input.
	double getAmplitude() const;

	//! Sets the dimensionality of the Gauss input.
	void setDimensionality(unsigned int dimensionality);

	//! Sets the size of the Gauss in the given dimension.
	void setSize(unsigned int dimension, unsigned int size);

    void eulerStep(const cedar::unit::Time& time);

	void setISI(double isi);

	void setMovementTime(double time);

	void setContinuous(bool isContinous);

	void setStopMoving(bool doesStop);

	void setStartBlank(bool startBlank);

	public slots:
	//!@brief a slot that is triggered if any of the Gauss function parameters are changed
	void updateMatrix();

	//!@brief a slot that is triggered if the matrix size is changed
	void updateMatrixSize();

	//!@brief a slot to process changes in dimensionality, including reinitializing the buffers
	void updateDimensionality();

	//--------------------------------------------------------------------------------------------------------------------
	// protected methods
	//--------------------------------------------------------------------------------------------------------------------
	protected:

	cedar::proc::DataSlot::VALIDITY determineInputValidity(cedar::proc::ConstDataSlotPtr /* slot */, cedar::aux::ConstDataPtr data) const;
	 void inputConnectionChanged(const std::string& inputName);

	//--------------------------------------------------------------------------------------------------------------------
	// private methods
	//--------------------------------------------------------------------------------------------------------------------
	private:
	//!@brief refreshes the internal matrix containing the Gaussian input


	void reset();
	void calculateOutput();

//	int* getSizeArray();

	//--------------------------------------------------------------------------------------------------------------------
	// members
	//--------------------------------------------------------------------------------------------------------------------
	protected:
	//!@brief the buffer containing the output
	cedar::aux::MatDataPtr mOutput;

	private:

	  cedar::aux::ConstMatDataPtr mStartTimer;
	  cedar::aux::ConstMatDataPtr mSwitchSignal;


	//--------------------------------------------------------------------------------------------------------------------
	// parameters
	//--------------------------------------------------------------------------------------------------------------------
	protected:
	//!@brief the amplitude of the Gauss function
	cedar::aux::DoubleParameterPtr _mAmplitude;
	//!@brief the time until the target changes
	cedar::aux::DoubleParameterPtr _mTimetilChange;
	//!@brief the dimensionality of the Gauss function
	cedar::aux::UIntParameterPtr _mDimensionality;
	//!@brief the vector of sigmas (i.e. width) of the Gauss function
	cedar::aux::DoubleVectorParameterPtr _mSigmas;
	//!@brief the vector of centers of the Gauss function
	cedar::aux::DoubleVectorParameterPtr _mCenters;
	//!@brief the vector of centers of the Gauss function
	cedar::aux::DoubleVectorParameterPtr _mSecondCenters;
	//!@brief the vector of sizes of matrix containing the Gauss function for each dimension
	cedar::aux::UIntVectorParameterPtr _mSizes;
	//!@brief determines if the Gauss input is cyclic or not
	cedar::aux::BoolParameterPtr _mIsCyclic;

//	cedar::aux::BoolParameterPtr _mStartBlank;
	cedar::aux::BoolParameterPtr _mIsContinuous;
	cedar::aux::BoolParameterPtr _mStopMoving;
	cedar::aux::DoubleParameterPtr _mMovementTime;
	cedar::aux::BoolParameterPtr _backAndForth;
	cedar::aux::BoolParameterPtr _externalSwitch;

	float elapsedTime=0;
	float curMovementTime = 0;
	float currentCenter;
	bool moveForward = true;
	bool isSwitch = false;

	std::vector<double> curCenter;
	private:


};

#endif /* TWOSTEPINPUT_H_ */
