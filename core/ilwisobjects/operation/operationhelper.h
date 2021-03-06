#ifndef OPERATIONHELPER_H
#define OPERATIONHELPER_H

namespace Ilwis {
class KERNELSHARED_EXPORT OperationHelper
{
public:
    OperationHelper();
    static void initialize(const IIlwisObject &inputObject, Ilwis::IIlwisObject &outputObject, IlwisTypes tp, quint64 what);
};
}

#endif // OPERATIONHELPER_H
