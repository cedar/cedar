/*
 * HarmonicOscillator.h
 *
 *  Created on: Apr 4, 2014
 *      Author: tekuejpp
 */

#ifndef CEDAR_DYN_HARMONIC_OSCILLATOR_H
#define CEDAR_DYN_HARMONIC_OSCILLATOR_H

#include "cedar/dynamics/steps/HarmonicOscillator.fwd.h"

#include "cedar/dynamics/Dynamics.h"
#include <cedar/processing/ExternalData.h> // getInputSlot() returns ExternalData
#include <cedar/processing/typecheck/DerivedFrom.h>
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include <vector>
#include <iostream>
#include <fstream>

//forward includes
#include "cedar/auxiliaries/MatData.fwd.h"





class cedar::dyn::steps::HarmonicOscillator : public cedar::dyn::Dynamics
{

	//	//--------------------------------------------------------------------------------------------------------------------
	//	// macros
	//	//--------------------------------------------------------------------------------------------------------------------
	Q_OBJECT

public:
	HarmonicOscillator();
	virtual ~HarmonicOscillator();
	void eulerStep(const cedar::unit::Time& time);

	cedar::aux::MatDataPtr getCurrentPosition();
	cedar::aux::MatDataPtr getCurrentVelocity();
	void setK(unsigned int dimension,double k);
	void setK(double k);
	void setB(unsigned int dimension,double B);
	void setB(double B);
	void setD(float D);
	std::vector<double> getK();
	std::vector<double> getB();
	void setInitPos(float dimension, float value);


	public slots:
	//!@brief a slot that is triggered if the matrix size is changed

	void updateMatrixSize();
	void updateDimensionality();
	void updateDampingK();
	void updateDampingB();


	private:
	cedar::proc::DataSlot::VALIDITY determineInputValidity(cedar::proc::ConstDataSlotPtr slot,cedar::aux::ConstDataPtr data) const;
	//!@brief Resets the field.
	void reset();
	private :


	cedar::aux::MatDataPtr mOutput;
	cedar::aux::MatDataPtr mOutputDot;
	cedar::aux::MatDataPtr mOutputDotDot;


	//!@brief the vector of sizes of matrix containing the Gauss function for each dimension
	cedar::aux::UIntParameterPtr _mDimensionality;
	cedar::aux::DoubleVectorParameterPtr  _initPos;
	cedar::aux::DoubleVectorParameterPtr _K;
	cedar::aux::DoubleVectorParameterPtr _B;
	cedar::aux::DoubleParameterPtr _D;
	cedar::aux::BoolParameterPtr _cyclicRestrictions;
	cedar::aux::DoubleVectorParameterPtr _lowerRestrictions;
	cedar::aux::DoubleVectorParameterPtr _upperRestrictions;



	cedar::aux::MatDataPtr _mPData;
	cedar::aux::MatDataPtr _mPDotData;

};

#endif /* CEDAR_DYN_HARMONIC_OSCILLATOR_H */
