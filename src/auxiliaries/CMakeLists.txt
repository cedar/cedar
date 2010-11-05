add_library(auxiliaries ${CEDAR_LIB_TYPE}
            Base.h
            Base.cpp
            Thread.h
            Thread.cpp
            ConfigurationInterface.h
            ConfigurationInterface.cpp
            LogFile.h
            LogFile.cpp
            exceptions/ExceptionBase.h
            exceptions/ExceptionBase.cpp
            exceptions/NullPointerException.h
            exceptions/NullPointerException.cpp
            exceptions/IndexOutOfRangeException.h
            exceptions/IndexOutOfRangeException.cpp
)
