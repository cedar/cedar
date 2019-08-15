// LOCAL INCLUDES
#include "Logarithm.h"
#include "cedar/processing/ElementDeclaration.h"
#include <math.h>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
    bool declare()
    {
        using cedar::proc::ElementDeclarationPtr;
        using cedar::proc::ElementDeclarationTemplate;

        ElementDeclarationPtr declaration
                (
                        new ElementDeclarationTemplate<cedar::proc::steps::Logarithm>
                                (
                                        "Algebra",
                                        "cedar.processing.steps.Logarithm"
                                )
                );
        declaration->setIconPath(":/steps/logarithm.svg");
        declaration->setDescription
                (
                        "Implements a logarithm function f(x)= log_base(x). "
                        "The "
                        "'base' can be provided via the GUI parameter "
                        "'base' "
                        "or optionally via "
                        "the according input (which then overrides the GUI parameter). "
                );

        declaration->declare();

        return true;
    }

    bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Logarithm::Logarithm()
        :
        cedar::proc::Step(false),
        mResult(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
        mBase(new cedar::aux::DoubleParameter(this,"base", 2.718281828459045))
{
    this->declareInput("input");
    this->declareInput("base (optional)", false); // optional

    this->declareOutput("result", mResult);

    QObject::connect(mBase.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Logarithm::recompute()
{
    cedar::aux::LogSingleton::getInstance()->message("recompute", CEDAR_CURRENT_FUNCTION_NAME);
    auto input = getInput("input");

    if (!input)
        return;

    auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

    if (!data)
        return;

    auto x= data->getData().clone();

    float base;
    bool  has_base_input = false;

    auto baseinput = getInput("base (optional)");
    if (baseinput)
    {
        auto basedata = boost::dynamic_pointer_cast<const cedar::aux::MatData>(baseinput);
        if (basedata
            && !basedata->getData().empty())
        {
            has_base_input= true;
            base= basedata->getData().at<float>(0, 0);
        }
    }

    if (!has_base_input)
    {
        base= mBase->getValue();
    }

    log(x, x);
    x = x/log(base);

    mResult->setData( x );

    checkOptionalInputs();
    // need to call this here, because determineinputValidity does not
    // get called for optional inputs that get removed by the user.
}

void cedar::proc::steps::Logarithm::compute(const cedar::proc::Arguments&)
{
    recompute();
}

void cedar::proc::steps::Logarithm::checkOptionalInputs()
{
}

//// validity check
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Logarithm::determineInputValidity
        (
                cedar::proc::ConstDataSlotPtr slot,
                cedar::aux::ConstDataPtr data
        ) const
{
    //all inputs have same type
    cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
    if( slot->getName() == "input" )
    {
        if (_input
            // && _input->getDimensionality() == 0
            && _input->getData().type() == CV_32F)
        {
            return cedar::proc::DataSlot::VALIDITY_VALID;
        }
    }

    // else
    return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::Logarithm::inputConnectionChanged(const std::string&)
{
    recompute();
}

void cedar::proc::steps::Logarithm::constantChanged()
{
    recompute();
}


