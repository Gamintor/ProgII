#include "header.h"

// Pokretanje sustava s dinamickom dodjelom memorije
void startajSustav() {
    kapacitet = MAX_BROJ_TERMINA;
    termini = calloc(kapacitet, sizeof(Termin));
    if(!termini) {
        perror("Neuspjesna alokacija memorije!\n");
        exit(EXIT_FAILURE);
    }
    brojTermina = 0;
}

// Oslobadanje resursa
void ocistiSustav() {
    if(termini) {
        free(termini);
        termini = NULL;
    }
    brojTermina = kapacitet = 0;
}

void prikaziIzbornik() {
    printf("\n====* Sustav za narucivanje kod lijecnika *====\n");
    printf("%d. Zakazi termin\n", ZAKAZI);
    printf("%d. Pregledaj termine\n", PREGLEDAJ);
    printf("%d. Izmjeni termin\n", IZMJENI);
    printf("%d. Otkazi termin\n", OTKAZI);
    printf("%d. Spremi u datoteku\n", SPREMI);
    printf("%d. Ucitaj datoteku\n", UCITAJ);
    printf("%d. Pretrazi po imenu i prezimenu pacijenta\n", PRETRAZI);
    printf("%d. Izlaz\n", IZLAZ);
}

// Poziv odgovarajucih funkcija ovisno o odabiru
void rukovanjeIzbornikom(Odabir odabir) {
    switch(odabir) {
        case ZAKAZI: 
            zakaziTermin();
            break;
        case PREGLEDAJ:
            pregledajTermine();
            break;
        case IZMJENI:
            izmjeniTermin();
            break;
        case OTKAZI:
            otkaziTermin();
            break;
        case SPREMI:
            spremiNaDatoteku();
            break;
        case UCITAJ:
            ucitajIzDatoteke();
            break;
        case PRETRAZI:
            pretraziPoImenu();
            break;
        case IZLAZ:
            printf("Izlaz iz sustava...\n");
            break;
        default:
            printf("Neispravan odabir!\n");
    }
}

void zakaziTermin() {
    if(brojTermina >= kapacitet) {
        printf("Nema vise slobodnih termina!\n");
        return;
    }

    Termin noviTer;

    printf("Unesi ime pacijenta: ");
    siguranUnosStringa(noviTer.pacijent, MAX_DUZINA);
    printf("Unesi ime lijecnika: ");
    siguranUnosStringa(noviTer.lijecnik, MAX_DUZINA);
    printf("Unesi datum i vrijeme (format: YYYY-MM-DD HH:MM): ");
    char datumVrijeme[20];
    siguranUnosStringa(datumVrijeme, sizeof(datumVrijeme));

    noviTer.datumVrijeme = obradiDatumVrijeme(datumVrijeme);
    if(noviTer.datumVrijeme == (time_t)-1) {
        printf("Neispravan format vremena i datuma!\n");
        return;
    }

    if(!jeliVrijemeDostupno(noviTer.datumVrijeme)) {
        printf("Vrijeme termina je zauzeto!\n");
        return;
    }
    noviTer.zauzet = true;
    termini[brojTermina++] = noviTer;
    sortirajTermine();
    printf("Termin je zakazan uspjesno!\n");
}

void pregledajTermine(void) {
    if(brojTermina == 0) {
        printf("Nema zakazanih termina!\n");
        return;
    }
    printf("\n====* Zakazani termini *====\n");
    for(int i = 0; i < brojTermina; i++) {
        if(termini[i].zauzet) {
            printf("%d. ", i + 1);
            ispisiTermin(&termini[i]);
        }
    }
}

void izmjeniTermin() {
    if(brojTermina == 0) {
        printf("Nema termina za azuriranje!\n");
        return;
    }

    int index;
    printf("Unesi broj termina za izmjenu: ");
    if(scanf("%d", &index) != 1 || index < 1 || index > brojTermina) {
        printf("Pogrešan broj termina!\n");
        while(getchar() != '\n');
        return;
    }
    // Ocisti preostale znakove u ulaznom bufferu
    while(getchar() != '\n');
    // prejdi na 0 bazirani indeks
    index--;
    printf("Odabrani termin:\n");
    ispisiTermin(&termini[index]);

    printf("Unesi novi datum i vrijeme (format: YYYY-MM-DD HH:MM): ");
    char vrijemeDatum[20];
    siguranUnosStringa(vrijemeDatum, sizeof(vrijemeDatum));

    time_t novoVrijeme = obradiDatumVrijeme(vrijemeDatum);
    if(novoVrijeme == (time_t)-1) {
        printf("Neispravan format datum/vremena!\n");
        return;
    }
    if(!jeliVrijemeDostupno(novoVrijeme)) {
        printf("Termin je vec popunjen!\n");
        return;
    }
    termini[index].datumVrijeme = novoVrijeme;
    sortirajTermine();
    printf("Termin uspjesno azuriran!\n");
}

void otkaziTermin() {
    if(brojTermina == 0) {
        printf("Nema termina za otkazivanje.\n");
        return;
    }    
    int index;
    printf("Unesi broj termina za otkazivanje: ");
    if(scanf("%d", &index) != 1 || index < 1 || index > brojTermina) {
        printf("Pogresan broj termina!\n");
        while(getchar() != '\n');
        return;
    }
    // Ocisti preostale znakove u ulaznom bufferu
    while(getchar() != '\n');
    // Prejdi na 0 bazirani indeks
    index--;
    // Pomakni elemente nakon indeksa
    for(int i = index; i < brojTermina - 1; i++) {
        termini[i] = termini[i + 1];
    }

    brojTermina--;
    printf("Termin uspjesno otkazan!\n");
}

void spremiNaDatoteku() {
    FILE *fajl = fopen(DATOTEKA, "w");
    if(!fajl) {
        perror("Neuspjesno otvaranje datoteke");
        return;
    }
    // Zapisi broj termina najprije
    if(fwrite(&brojTermina, sizeof(int), 1, fajl) != 1) {
        perror("Nespjesan zapis broja termina");
        fclose(fajl);
        return;
    }
    // Zapisi termine
    if(fwrite(termini, sizeof(Termin), brojTermina, fajl) != brojTermina) {
        perror("Neuspjesan zapis termina");
        fclose(fajl);
        return;
    }
    fclose(fajl);
    printf("Termini uspjesno spremljeni na datoteku!\n");
}

void ucitajIzDatoteke() {
    FILE *fajl = fopen(DATOTEKA, "r");
    if(!fajl) {
        perror("Neuspjesno otvaranje datoteke za citanje");
        return;
    }
    // Ocitaj broj termina najprije
    int broj;
    if(fread(&broj, sizeof(int), 1, fajl) != 1) {
        perror("Neuspjesno ocitanje broja termina");
        fclose(fajl);
        return;
    }
    
    if(broj > kapacitet) {
        // Dodijeli veci blok memorije ukoliko je potrebno
        Termin *noviTermini = (Termin*)realloc(termini, sizeof(Termin) * broj);
        if(!noviTermini) {
            perror("Neuspjela dodjela memorije!");
            fclose(fajl);
            return;
        }
        termini = noviTermini;
        kapacitet = broj;
    }
    if(fread(termini, sizeof(Termin), broj, fajl) != broj) {
        perror("Greska pri citanju datoteke");
        fclose(fajl);
        return;
    }

    brojTermina = broj;
    fclose(fajl);
    printf("Termini uspjesno ucitani iz datoteke!\n"); 
}

void ispisiTermin(const Termin *termin) {
    char buffer[20];
    strftime(buffer, sizeof(buffer), FORMAT_DATUM_VRIJEME, localtime(&termin->datumVrijeme));
    printf("Pacijent: %s, Doktor: %s, Vrijeme: %s\n", termin->pacijent, termin->lijecnik, buffer);
}

bool jeliVrijemeDostupno(time_t vrijeme) {
    for(int i = 0; i < brojTermina; i++) {
        if(termini[i].zauzet && difftime(vrijeme, termini[i].datumVrijeme) == 0) {
            return false;
        }
    }
    return true;
}


void sortirajTermine() {
    qsort(termini, brojTermina, sizeof(Termin), usporediTermine);
}

int usporediTermine(const void *a, const void *b) {
    const Termin *terminA = (const Termin*)a;
    const Termin *terminB = (const Termin*)b;
    return difftime(terminA->datumVrijeme, terminB->datumVrijeme);
}

void siguranUnosStringa(char *buffer, int velicina) {
    if (fgets(buffer, velicina, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    // Ukloni newline na kraju stringa
    int len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    } else {
        // Ocisti preostale znakove u ulaznom bufferu
        while(getchar() != '\n');
    }
}

// Pretvorba iz stringa u time_t tip podatka
time_t obradiDatumVrijeme(const char *datum) {
    struct tm tm = {0};
    int godina, mjesec, dan, sat, minute;
   
    // Provjera duljine stringa, ocekujemo "YYYY-MM-DD HH:MM"
    if (strlen(datum) != 16) {
        return (time_t)-1;
    }
    
    if (sscanf(datum, "%d-%d-%d %d:%d", &godina, &mjesec, &dan, &sat, &minute) != 5) {
        return (time_t)-1;
    }
    
    // Provjera raspona vrijednosti
    if (godina < 1900 || mjesec < 1 || mjesec > 12 || dan < 1 || dan > 31 ||
        sat < 0 || sat > 23 || minute < 0 || minute > 59) {
        return (time_t)-1;
    }
    
    // Dodatna provjera za broj dana u mjesecu
    if ((mjesec == 4 || mjesec == 6 || mjesec == 9 || mjesec == 11) && dan > 30) {
        return (time_t)-1;
    }
    
    // Provjera za veljacu
    if (mjesec == 2 && dan > 28) {
        return (time_t)-1;
    }
    
    // Popuni tm strukturu
    tm.tm_year = godina - 1900; // Godine nakon 1900
    tm.tm_mon = mjesec - 1;
    tm.tm_mday = dan;
    tm.tm_hour = sat;
    tm.tm_min = minute;
    tm.tm_isdst = -1;

    time_t rezultat = mktime(&tm);

    if (rezultat == (time_t)-1) {
        return (time_t)-1;
    }
    
    return rezultat;
}

int usporediImenaPacijenta(const void* a, const void* b) {
    const Termin* terA = (const Termin*)a;
    const Termin* terB = (const Termin*)b;
    return strcmp(terA->pacijent, terB->pacijent);
}

void pretraziPoImenu() {
    if (brojTermina == 0) {
        printf("Nema termina za pretragu.\n");
        return;
    }
    // Stvorimo kopiju termina za pretragu
    Termin *kopijaTermini = malloc(sizeof(Termin) * brojTermina);
    memcpy(kopijaTermini, termini, sizeof(Termin) * brojTermina);
    // Sortiramo ga po imenima pacijenta
    qsort(kopijaTermini, brojTermina, sizeof(Termin), usporediImenaPacijenta);

    char trazenoIme[MAX_DUZINA];
    printf("Unesi ime i prezime pacijenta za pretragu: ");
    siguranUnosStringa(trazenoIme, MAX_DUZINA);

    Termin *rezulatat = bsearch(trazenoIme, kopijaTermini, brojTermina, sizeof(Termin), usporediImenaPacijenta);

    if(rezulatat) {
        printf("\n===* Svi pronadeni termini za %s *===\n ", trazenoIme);
        for(int i = 0; i < brojTermina; i++) {
            if(strcmp(termini[i].pacijent, trazenoIme) == 0) {
                ispisiTermin(&termini[i]);
            }
        }
    } else {
        printf("Nema termina za trazenog pacijenta!\n");
    }
    free(kopijaTermini);
}