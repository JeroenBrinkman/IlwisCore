#ifndef MODULE_H
#define MODULE_H

#include <QObject>
#include <QMap>
#include "Kernel_global.h"

namespace Ilwis {

class ICommandInfo;
struct ExecutionContext;

}

namespace Ilwis {

struct ExecutionContext;
class ICommands;
class Module;

/*!
  the baseclass for all plugins in Ilwis

 All plugins in Ilwis are derived from this class. The QT plugin loader uses this class to identify the plugins

*/
class KERNELSHARED_EXPORT Module : public QObject
{
    Q_OBJECT
    //Q_INTERFACES(Ilwis::Module)
public:
    explicit Module(QObject *parent, const QString& name, const QString& ivers, const QString& vers);
    ~Module();
    /*!
      This method identifies the interface version of the plug-in

     version and interface version are different concepts. A new version of a plug-in might have the same interface version
     but a different version number.

     \return QString version id
    */
    QString getInterfaceVersion() const;
    /*!
     *  returns the name of the module
     * \return name
     */
    QString name() const;
    /*!
     * returns the version of the module
     *
     * \return version
     */
    QString version() const;
    /*!
     *  returns name and version combined of the module
     *
     *  convenience function as the combination of these two is used to uniquely identify the module
     * \return
     */
    QString getNameAndVersion() const;
     /*!
     *  prepare initialization routine of the module
     *
     *overruled in deravatives. Usually operations, creation methods for factories and other global accessible functions are transferred here to the system context. It depends on the module
     */
    virtual void prepare();
protected:

private:
    QString _moduleName;
    QString _interfaceVersion;
    QString _version;


signals:

public slots:

};


class ModuleMap : public QMap<QString, Module *> {
public:
    ~ModuleMap();
    void addModules();
    void initModules();
private:

};
}

Q_DECLARE_INTERFACE(Ilwis::Module,
                    "n52.org.ilwis.plugin.ilwis.moduleinterface/1.0")






#endif // MODULE_H
