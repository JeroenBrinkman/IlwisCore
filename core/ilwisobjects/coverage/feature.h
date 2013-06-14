#ifndef FEATURE_H
#define FEATURE_H

#include "Kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT FeatureInterface {
public:
    virtual ~FeatureInterface() {}
    virtual quint32 itemId() const=0 ;
    virtual void itemId(quint32 v) = 0;
    virtual quint64 featureid() const = 0;
    virtual bool isValid() const = 0 ;
    virtual const Geometry& geometry(quint32 index=0) const = 0;
    virtual void add(const Geometry& geom) = 0;
    virtual void attributeRecord(const SPAttributeRecord& record) = 0;
    QVariant operator()(const QString& name, int index = -1);

protected:
    virtual QVariant value(const QString& name, int index=-1) = 0;

};

/*!
The feature class represents a spatial object with a single identity and a one or more geometries. This is different from the regular
definition of a feature that has only one geometry. The reasoning for this is that an object that moves trough time is still the same
object. Same is true for objects at different scales. To be able to represent these kind of data structures a feature can have multiple
geometries organized in a vector. The index in the vector has meaning ( similar to z direction in gridcoverages) and uses the verticaldomain.
 */
class KERNELSHARED_EXPORT Feature : public FeatureInterface {
    friend class FeatureCoverage;
    friend class FeatureIterator;

public:
    Feature();
    Feature(quint32 v) ;
    ~Feature();

    /*!
     Features are tied to a domain to give them meaning.
     * \return  the id of the domain item that is tied to this feature
     */
    quint32 itemId() const ;
    /*!
     Features are tied to a domain to give them meaning.
     * \param v sets the domain item to which this feature is bound
     */
    void itemId(quint32 v);
    quint64 featureid() const;
    bool isValid() const ;
    const Geometry& geometry(quint32 index=0) const;
    void add(const Geometry& geom);
    void attributeRecord(const SPAttributeRecord& record);

private:
    Feature(const Feature& f) ; // nocopy constructor, _featureid is unique
    Feature& operator=(const Feature& f) ; // no assignment , _featureid is unique
    QVariant value(const QString& name, int index=-1);
    static quint64 _idbase;
    quint32 _itemid; // from the domain
    quint64 _featureid; // unique
    std::vector<Geometry> _track;
    SPAttributeRecord _record;
    const Geometry _invalidGeom;

};

typedef QSharedPointer<Feature> SPFeature;

class KERNELSHARED_EXPORT FeatureProxy : public FeatureInterface {
public:
    FeatureProxy();
    ~FeatureProxy();
    quint32 itemId() const ;
    void itemId(quint32 v) ;
    quint64 featureid() const ;
    bool isValid() const  ;
    const Geometry& geometry(quint32 index=0) const ;
    void add(const Geometry& geom) ;
    void attributeRecord(const SPAttributeRecord& record) ;
    void setProxy(SPFeature f, quint32 index);
protected:
    QVariant value(const QString& name, int index=-1);
private:
    SPFeature _feature;
    quint32 _trackIndex;
    const Geometry _invalidGeom;
};

bool operator==(const Feature& f1, const Feature& f2) ;

}
Ilwis::Feature *createFeature(quint64 itemId);
#endif // FEATURE_H
