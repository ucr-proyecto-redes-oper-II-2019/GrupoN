#ifndef TCPL_1
#define TCPL_1
#include <iostream>
using namespace std;
class TCPL{
private:
    int conteo;
    int aviso;
public:
    TCPL(int aviso){
        conteo = 0;
        this->aviso = aviso;
    };
    ~TCPL(){};
    int suma(int x, int y){
        return x+y;
    };
    void aumentar(){
        conteo++;
        cout << "CONTEO : " << conteo << endl;
        if (conteo == aviso) {
            cout << "MENSAJE DE AVISO" << endl;
        }
    };
};

#endif
