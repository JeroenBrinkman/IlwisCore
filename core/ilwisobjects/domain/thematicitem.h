#ifndef THEMATICITEM_H
#define THEMATICITEM_H

#include "Kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT ThematicItem : public NamedIdentifier
{
public:

    ThematicItem();
    //ThematicItem(const QString &name, const QString &cde=sUNDEF, const QString &descr=sUNDEF);
    ThematicItem(const QStringList &parts);

    QString description() const;
    QString code() const;
    void description(const QString &desc);
    void code(const QString &code);
    DomainItem *clone() const;
    static IlwisTypes valueTypeS();
    IlwisTypes valueType() const;

private:
    QString _description;
    QString _code;
};

typedef IlwisData<ItemDomain<ThematicItem> > IThematicDomain;
typedef QSharedPointer<ThematicItem> SPThematicItem;

}

bool operator == (const Ilwis::ThematicItem& item1, const Ilwis::ThematicItem& item2);


#endif // THEMATICITEM_H
