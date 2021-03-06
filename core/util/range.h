#ifndef RANGE_H
#define RANGE_H

#include <QSharedPointer>
#include "Kernel_global.h"

namespace Ilwis {

/*!
 * \brief The Range class base interface for all objects that need to define a range of values.
 *
 *Examples of dervatives of this class are numerical ranges, thematical collections, time intervals. The class itself has not that usefull an interface but pointers to this class can be used as a
 *a general container for more specialized ranges .
 */

class KERNELSHARED_EXPORT Range
{
public:
    Range();
    virtual ~Range() {}

    /*!
     determines the validity of a range. This method must be implemented by each deravative
     * \return true for a valid range
     */
    virtual bool isValid() const = 0;
    /*!
    translates the string into a string representation. As each type of range has a different string representation this method has to be reimplemented by each derivative
     * \return a string
     */
    virtual QString toString() const = 0;
    /*!
     * \brief valueType returns the type of values contained in the range
     * \return an ilwistype
     */
    virtual IlwisTypes valueType() const;
    /*!
     * \brief determineType determines the minimal range needed type for this range.
     *
     *For example a range of numbers can be in the 32 bits range while the range contains only the numbers between 0-255, so the minimal range would be itUINT8
     * \return an ilwistype
     */
    virtual IlwisTypes determineType() const { return itUNKNOWN;}
     /*!
     copies the range to a new instance. This pointer has to be deleted by the caller
     * \return the new Range
     */
    virtual double ensure(double, bool inclusive = true) const { return rUNDEF; }
    virtual Range *clone() const = 0;
    virtual bool contains(const QString& value, bool inclusive = true) const = 0;

    virtual bool isContinuous() const;
    void interpolation(const QString& ip);
    virtual QString interpolation() const;
protected:
    IlwisTypes _vt;
    QString _interpolation;



};
typedef QSharedPointer<Range> SPRange;
}

#endif // RANGE_H
