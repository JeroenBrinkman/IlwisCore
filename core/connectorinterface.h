#ifndef ICONNECTOR_H
#define ICONNECTOR_H

#include "Kernel_global.h"
#include <QExplicitlySharedDataPointer>

namespace Ilwis {

class IlwisObject;
class ConnectorFactory;
class Resource;
class Grid;
class OperationExpression;
class ExecutionContext;

/*!
 * \brief The ConnectorInterface class all connectors are derived from this interface
 *
 *The connector interface enumerates the methods that may be implemented by a derivative. It is unlikely that a derivative will implement all methods so a default implementation is provied.
 *Usualy the MetaData methods are implemented and depending on the nature of the source also one or more of the binary (Grid/table) ones is implemented.
 */
class KERNELSHARED_EXPORT ConnectorInterface {
public:
    virtual ~ConnectorInterface() {}
    /*!
     \brief loads the metadata and non binary information into the ilwisobject. After this method it should be a valid object

     \param data the ilwisobject to be initialized
     \return bool true when succesfull. If not succesful the reason can be found in the issuelogger
    */
    virtual bool loadMetaData(IlwisObject* ) { return false;}

    /*!
     * \brief loadGridData loads the 2D or 3D raster data from the source into memory
     *
     *Grid data can be of any size as the memory manager of Ilwis will swap any excess out to its disk cache.
     *Note that Ilwis doesnt make a difference between 2D and 3D griddata. If the local format has different storage methods for those they have to be resolved at connector level
     * \return returns the allocated grid
     */
    virtual Grid* loadGridData(IlwisObject*) { return 0;}
    /*!
     * \brief loadBinaryData loads bulk binary data into the ilwisobject.
     * \return true when succesfull. If not succesful the reason can be found in the issuelogger
     */
    virtual bool loadBinaryData(IlwisObject* ) { return false; }
    virtual bool store(IlwisObject *, int ) { return false; }

    virtual bool execute(const OperationExpression& , ExecutionContext *) { return false; }
    /*!
     * \brief isReadOnly returns if a connector can write to a source.
     *
     *This return is more of the physical possibility of writing data. There is also a readonly flag at ilwisobject level but describes usualy the logical readonly status (e.g. I dont want that this will be written)
     * \return
     */
    virtual bool isReadOnly() { return true; }

    /*!
     * \brief clone copies the connector for use with another object
     *
     *A connector is dedicated to one IlwisObject only. If one needs an identical connector for whatever reason one can clone it and reuse it
     * \return a new connector
     */
    virtual ConnectorInterface* clone() const { return 0; }
    /*!
     * \brief source returns the resource location were this connector is attached to
     * \return the url of the source
     */
    virtual Resource& source() = 0;
    virtual QString provider() const = 0;
    virtual void format(const QString&) {}
    virtual QString format() const { return sUNDEF; }

};


typedef ConnectorInterface* (*ConnectorCreate)(const Resource& resource, bool load);
}

#endif // ICONNECTOR_H
