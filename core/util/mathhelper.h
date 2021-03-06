#ifndef MATHHELPER_H
#define MATHHELPER_H

namespace Ilwis {

struct Coefficient{
    double x,y;
};

class MathHelper
{
public:
    MathHelper();
    static bool findOblique(int iPoints, const std::vector<Coordinate>& independent, const std::vector<Coordinate>& dependent, std::vector<Coefficient>& coef, bool useCols);
    static bool findPolynom(int iTerms, int iPoints, const std::vector<Coordinate>& independent, const std::vector<Coordinate>& dependent, std::vector<Coefficient>& coef);
};
}
#endif // MATHHELPER_H
