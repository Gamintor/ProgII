#include "header.h"

int main() {
    startajSustav();

    Odabir odabir;
    do {
        prikaziIzbornik();
        printf("Unesite vas odabir: ");
        scanf("%d", (int*)&odabir);
        while(getchar() != '\n'); // Ocisti ulazni buffer 
        
        rukovanjeIzbornikom(odabir);
    } while(odabir != IZLAZ);

    ocistiSustav();

    return 0;
}