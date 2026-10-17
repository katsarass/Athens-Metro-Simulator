#pragma once

#include <vector>
#include <string>
#include <utility>
#include <memory>
#include "sgg/graphics.h"

// domi stathmou metro
struct Stathmos {
    int id;
    std::string onoma;
    int grammi;        // 1=prasini, 2=kokkini, 3=mple
    float x, y;        // thesi sto canvas
};


// forward declaration gia polymorfismo
class AntikeimenoSkinis;


// se poia fasi vrisketai h efarmogi
enum class Katastasi {
    EpilogiArxis,
    EpilogiStoxou,
    Etoimo
};

class GlobalState {
public:
    // singleton (ena mono stigmiotypo)
    static GlobalState& getInstance();


    // akyrosi copy constructors gia na min antigrafetai h klasi
    GlobalState(const GlobalState&) = delete;
    GlobalState& operator=(const GlobalState&) = delete;
    GlobalState(GlobalState&&) = delete;
    GlobalState& operator=(GlobalState&&) = delete;


    ~GlobalState(); // ylopoiisi sto .cpp


    // vasikes methodoi poy zhtaei
    void init();
    void update(float ms);
    void draw();


    // methodoi sxediasis (kalountai apo ta antikeimena skinis)
    void zwgrafiseGrammes();
    void zwgrafiseStathmous();
    void zwgrafiseUI();


    // getters gia plirofories
    int  getArxi() const { return dialegmeniArxi; }
    int  getStoxo()  const { return dialegmenosStoxos; }
    bool einaiEtoimiDiadromi() const { return exeiDiadromi; }

    std::string keimenoDiadromis() const;

private:
    GlobalState() = default;

    // eswterikes methodoi
    void ftiakseXarti();
    void ftiakseGeitones();
    void ftiakseSkini();
    void elegxosPontikiou();

    bool einaiTermatikos(int idx) const;

    // dijkstra algoririthmos
    bool vresDiadromiDijkstra(int arxiIdx, int telosIdx);
    void ftiakseAkmesDiadromis();
    bool einaiAkmiDiadromis(int a, int b) const;




private:
    // dedomena xarti
    std::vector<Stathmos> stathmoi;
    std::vector<std::pair<int, int>> akmes;
    std::vector<std::vector<std::pair<int, float>>> geitones;

    // metavlites diadromis
    bool exeiDiadromi = false;
    std::vector<int> komvoiDiadromis;
    std::vector<std::pair<int, int>> akmesDiadromis;


    // epiloges user
    int dialegmeniArxi = -1;
    int dialegmenosStoxos = -1;


    // mouse
    int stathmosPontikiou = -1; // poion stathmo koitaei to pontiki
    float pontikiX = 0.0f;
    float pontikiY = 0.0f;
    bool proigoumenoKlik = false;


    // koumpia UI
    float koubibtnX = 1100, koubibtnY = 40, koubibtnW = 150, koubibtnH = 50;
    float koubibtnFindX = 1100, koubibtnFindY = 100, koubibtnFindW = 150, koubibtnFindH = 50;

    // polymorfiki lista antikeimenwn
    std::vector<std::unique_ptr<AntikeimenoSkinis>> skini;


    Katastasi katastasiApp = Katastasi::EpilogiArxis;
};