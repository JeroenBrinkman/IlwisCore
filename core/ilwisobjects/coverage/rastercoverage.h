#ifndef RASTERLAYER_H
#define RASTERLAYER_H

#include "Kernel_global.h"

namespace Ilwis {

class Resource;
class Grid;

class KERNELSHARED_EXPORT RasterCoverage : public Coverage
{
public:

    friend class PixelIterator;
    friend class GridBlock;
    friend class Grid;
    friend class RasterInterpolator;

    RasterCoverage();
    RasterCoverage(const Resource& resource);
    ~RasterCoverage();

    IlwisTypes ilwisType() const;
    virtual RasterCoverage *copy() ;

    const DataDefinition& datadef() const;
    DataDefinition& datadef();
    const Ilwis::IGeoReference &georeference() const;
    void georeference(const IGeoReference& grf) ;
    Size size() const;
    void size(const Size& sz);

    void copyBinary(const IlwisData<RasterCoverage> &raster, int index);

    double coord2value(const Coordinate &c){
        if ( _georef->isValid() && c.isValid()) {
            Point2D<double> pix = _georef->coord2Pixel(c);
            return pix2value(pix);
        }
        return rUNDEF;
    }

    double pix2value(const Point3D<double>& pix){
        if ( _georef->isValid() && !connector().isNull()) {
            if ( _grid.isNull()) {
                _grid.reset(connector()->loadGridData(this));
                if (_grid.isNull())
                    return rUNDEF;
            }

            double v = _grid->value(pix);
            return datadef().range()->ensure(v);
        }
        return rUNDEF;
    }


    Resource source(int mode=cmINPUT) const;
    void unloadBinary();
protected:
    Grid *grid();
    QScopedPointer<Grid> _grid;
    void copyTo(IlwisObject *obj);

private:
    DataDefinition _datadefCoverage;
    IGeoReference _georef;
    Size _size;
    std::mutex _mutex;
};

typedef IlwisData<RasterCoverage> IRasterCoverage;
}

Q_DECLARE_METATYPE(Ilwis::IRasterCoverage)


#endif // RASTERLAYER_H
