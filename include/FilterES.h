#ifndef FILTERES_H
#define FILTERES_H
#include <vector>

using namespace std;

class FilterES {
    const double Alpha;
    int nSize;
    std::vector<double> arVect;
    std::vector<double>::iterator it, end1, begin1;
  public:
    FilterES(int, double);
    double FilterAnalog(double);
    double getValue();
};

#endif // FILTERES_H