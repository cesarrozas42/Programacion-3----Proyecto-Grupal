#ifndef PROYECT_PRGR3_AVANCE_TAG_H
#define PROYECT_PRGR3_AVANCE_TAG_H

#include <string>
using namespace std;
class Tag {
private:
    string tipo;
    string valor;

public:
    Tag();
    Tag(string tipo, string valor);

    string getTipo() const;
    string getValor() const;
};


#endif
