#include <QDataStream>
#include "ilwis.h"
#include "serializationoptions.h"
#include "numericrange.h"

using namespace Ilwis;

NumericRange::NumericRange() : _min(0), _max(-1),_step(0) {

}
NumericRange::NumericRange(double mi, double ma, double step) : _step(step), _undefined(rUNDEF) {
    setMin(mi);
    setMax(ma);
}
NumericRange::NumericRange(const NumericRange &vr): _undefined(rUNDEF)
{
    set(vr);
}

bool NumericRange::isValid() const
{
    return _min <= _max ;
}

Range *NumericRange::clone() const {
    NumericRange *rng = new NumericRange(_min, _max, _step);
    rng->interpolation(interpolation());
    return rng;
}

bool NumericRange::contains(double v, bool inclusive) const
{
    if (!isValid())
        return false;

    if ( inclusive)
        return v >= _min && v <= _max;
    return v > _min && v < _max;

}

double NumericRange::max() const
{
    return _max;
}
double NumericRange::min() const
{
    return _min;
}
void NumericRange::setMin(double v)
{
    if (_step == 1){
        _min = (qint64) v;
    } else
        _min = v;
}

double NumericRange::distance() const
{
    if ( !isValid())
        return rUNDEF;

    return _max - _min;
}

void NumericRange::setMax(double v)
{
    if (_step == 1){
        _max = (qint64) v;
    } else
        _max = v;
}

void NumericRange::setStep(double step) {
    _step = step;
}

double NumericRange::step() const {
    return _step;
}

NumericRange& NumericRange::operator+=(double v)
{
    if ( !isValid()){
        setMin(v);
        setMax(v);
    }
    else {
        if ( v > _max )
            setMax(v);
        if ( v < _min)
            setMin(v);
    }
    return *this;
}

bool NumericRange::operator==(const NumericRange& vr) {
    return vr.max() == max() && vr.min() == min() && vr.step() == step();
}

bool NumericRange::operator<(const NumericRange &vr)
{
    return max() < vr.max() && min() < vr.min();
}

bool NumericRange::operator>(const NumericRange &vr)
{
    return max() > vr.max() && min() > vr.min();
}

QString NumericRange::toString() const {
    if(!isValid())
        return "? ? ?";
    long n1 = significantDigits(_min);
    long n2 = significantDigits(_max);
    long n = std::max(n1, n2);
    bool isfloat = _step == 0 || _step - (quint64)_step != 0;
    if (isfloat){
        QString rng = QString::number(_min,'f',n);
        rng += ' ';
        rng += QString::number(_min,'f',n);
        if ( _step != 0){
            rng += ' ';
            rng += QString::number(_step) ;
        }
        return rng;
    }
    QString rng = QString("%1 %2 %3").arg(_min).arg(_max).arg(_step);
    return rng;


}
void NumericRange::set(const NumericRange& vr)
{
    _step = vr._step;
    setMin(vr._min);
    setMax(vr._max);
}

double NumericRange::ensure(double v, bool inclusive) const
{
    if ( !contains(v, inclusive))
        return _undefined;
    if ( (_step - (int)_step) == 0.0)
        return (qint32)(v + 0.5);
    return v;
}

bool NumericRange::contains(const QString &value, bool inclusive) const
{
    bool ok;
    double v = value.toDouble(&ok);
    if (!ok)
        return false;
    if ( v == rUNDEF)
        return false;
    return contains(v, inclusive);
}

bool NumericRange::contains(SPRange rng, bool inclusive) const
{
    if ( rng.isNull())
        return false;
    SPNumericRange numrange = rng.staticCast<NumericRange>();
    bool ok = contains(numrange->min(), inclusive);
    return ok && contains(numrange->max(), inclusive);
}

bool NumericRange::contains(NumericRange *rng, bool inclusive) const
{
    if ( !rng)
        return false;
    bool ok = contains(rng->min(), inclusive);
    return ok && contains(rng->max(), inclusive);
}


long NumericRange::significantDigits(double m1) const{
    if ( fabs(m1) > 1e30)
        return 100;

    QString s = QString::number(m1);
    for(int i=s.size() - 1; i != 0; --i ) {
        QChar c = s[i];
        if ( c != '0') {
            if ( s.indexOf(".") > 0) // '.' is not counted for significant numbers
                return i;
            return i -1;
        }
    }
    return s.size();
}

IlwisTypes NumericRange::determineType() const{

    IlwisTypes vt = itUNKNOWN;
    if ( _step == 1) { // integer part
        bool sig = min() < 0;
        if ( max() <=128 && sig)
            vt =  itINT8;
        else if ( max() <= 255 && !sig)
            vt =  itUINT8;
        else if ( max() <= 32768 && sig)
            vt =  itINT16;
        else if ( max() <= 65536 && !sig)
            vt =  itUINT32;
        else if ( max() <= 2147483647 && sig)
            vt =  itINT32;
        else if ( max() <= 4294967296 && !sig)
           vt =  itUINT32;
        else
            vt =  itINT64; // unlikely but anyway
    } else { // real part
        int signif1 = std::max(significantDigits(min()), significantDigits(max()));
        if ( signif1 > 6)
           vt =  itDOUBLE;
        else
            vt =  itFLOAT;
    }
    return vt;
}

NumericRange *NumericRange::merge(const QSharedPointer<NumericRange> &nr1, const QSharedPointer<NumericRange> &nr2)
{
    return new NumericRange(std::min(nr1->min(), nr2->min()),
                            std::max(nr1->max(), nr2->max()),
                            std::min(nr1->step(), nr2->step()));
}

