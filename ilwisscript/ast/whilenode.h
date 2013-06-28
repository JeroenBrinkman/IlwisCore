#ifndef WHILENODE_H
#define WHILENODE_H

namespace Ilwis {
class WhileNode : public ASTNode
{
public:
    WhileNode();
    void setCondition(ExpressionNode *expr);
    QString nodeType() const;


    bool evaluate(SymbolTable &symbols, int scope);
private:
    QSharedPointer<ExpressionNode> _condition;

    bool checkCondition(Ilwis::SymbolTable &symbols, int scope);

};
}

#endif // WHILENODE_H
