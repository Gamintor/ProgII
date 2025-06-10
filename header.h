#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

#pragma once
#ifndef HEADER
#define HEADER

// Konstante
#define MAX_DUZINA 40
#define MAX_BROJ_TERMINA 100
#define FORMAT_DATUM_VRIJEME "%Y-%m-%d %H:%M"
#define DATOTEKA "termini.txt"

// Enum za odabire u izborniku
typedef enum {
    IZLAZ,
    ZAKAZI,
    PREGLEDAJ,
    IZMJENI,
    OTKAZI,
    SPREMI,
    UCITAJ
} Odabir;

typedef struct {
    char pacijent[MAX_DUZINA];
    char lijecnik[MAX_DUZINA];
    time_t datumVrijeme;
    bool zauzet;
} Termin;

static Termin *termini = NULL;
static int brojTermina = 0;
static int kapacitet = 0;

void startajSustav();
void ocistiSustav();
void prikaziIzbornik();
void rukovanjeIzbornikom(Odabir odabir);
void zakaziTermin();
void pregledajTermine();
void ispisiTermin(const Termin *termin);
void izmjeniTermin();
void otkaziTermin();
void spremiNaDatoteku();
void ucitajIzDatoteke();
bool jeliVrijemeDostupno(time_t vrijeme);
void sortirajTermine();
int usporediTermine(const void *a, const void *b);
void siguranUnosStringa(char *buffer, int velicina);
time_t obradiDatumVrijeme(const char *datum);

#endif