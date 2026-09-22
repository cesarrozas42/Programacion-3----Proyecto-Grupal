#include "Tag.h"

Tag::Tag() {
    tipo = "";
    valor = "";
}

Tag::Tag(string tipo, string valor) {
    this->tipo = tipo;
    this->valor = valor;
}

string Tag::getTipo() const {
    return tipo;
}

string Tag::getValor() const {
    return valor;
}
