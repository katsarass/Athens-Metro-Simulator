#include "GlobalState.h"
#include <cmath>
#include <limits>
#include <algorithm>

namespace
{
    
    const float OTHONI_W = 1200.0f;
    const float OTHONI_H = 700.0f;


    // aktina gia to klik tou pontikiou
    const float AKTINA_KLIK = 20.0f;
    const float AKTINA_KLIK_SQ = AKTINA_KLIK * AKTINA_KLIK;


    // mege8os stathmwn 
    const float AKTINA_STATHMOU = 13.0f;
    const float DAKTYLIOS_HOVER = 18.0f;


    // stathmoi ana seira gia na min paei ape3w
    const int STATHMOI_ANA_GRAMMI = 5;


    // voithitiki gia na doume an mouse einai mesa se kouti
    bool mesaStoKouti(float px, float py, float cx, float cy, float w, float h)
    {
        return (px >= cx - w / 2 && px <= cx + w / 2 &&
            py >= cy - h / 2 && py <= cy + h / 2);
    }
}



// vasiki klasi gia layers (polymorfismos edw)
class AntikeimenoSkinis {
public:
    virtual void update(GlobalState&, float) {}
    virtual void draw(GlobalState& gs) = 0;
    virtual ~AntikeimenoSkinis() = default;
};




// layer gia grammes (akmes)
class LayerGrammon : public AntikeimenoSkinis {
public:
    void draw(GlobalState& gs) override { gs.zwgrafiseGrammes(); }
};




// layer gia kyklous stathmwn
class LayerStathmon : public AntikeimenoSkinis {
public:
    void draw(GlobalState& gs) override { gs.zwgrafiseStathmous(); }
};




// layer gia UI (koumpia, keimena)
class LayerUI : public AntikeimenoSkinis {
public:
    void draw(GlobalState& gs) override { gs.zwgrafiseUI(); }
};



GlobalState::~GlobalState() = default;



// singleton pattern opws zhtithike
GlobalState& GlobalState::getInstance()
{
    static GlobalState instance;
    return instance;
}



void GlobalState::init()
{
    // arxikopoihsh xarti kai grafou
    ftiakseXarti();
    ftiakseGeitones();

    exeiDiadromi = false;
    komvoiDiadromis.clear();
    akmesDiadromis.clear();

    // ftiaxoume ta layers gia to draw
    ftiakseSkini();
    katastasiApp = Katastasi::EpilogiArxis;
}



void GlobalState::ftiakseSkini()
{
    skini.clear();
    // vazoume ta layers sti lista (polymorfika)
    skini.emplace_back(std::make_unique<LayerGrammon>());
    skini.emplace_back(std::make_unique<LayerStathmon>());
    skini.emplace_back(std::make_unique<LayerUI>());
}



void GlobalState::update(float ms)
{
    // tsekaroume mouse
    elegxosPontikiou();

    // kaloume update se ola ta antikeimena
    for (auto& antikeimeno : skini) {
        antikeimeno->update(*this, ms);
    }
}


void GlobalState::draw()
{
    graphics::Brush bg;
    // background
    bg.fill_color[0] = 0.80f; bg.fill_color[1] = 0.80f; bg.fill_color[2] = 0.80f;
    bg.fill_opacity = 1.0f;
    bg.outline_opacity = 0.0f;
    graphics::drawRect(OTHONI_W / 2.0f, OTHONI_H / 2.0f, OTHONI_W, OTHONI_H, bg);

    // polumorfiki klisi draw
    for (auto& antikeimeno : skini) {
        antikeimeno->draw(*this);
    }
}





void GlobalState::ftiakseGeitones()
{
    geitones.clear();
    geitones.resize(stathmoi.size());

    // lambda gia apostasi
    auto vresApostasi = [&](int a, int b) {
        const float dx = stathmoi[a].x - stathmoi[b].x;
        const float dy = stathmoi[a].y - stathmoi[b].y;
        return std::sqrt(dx * dx + dy * dy);
        };

    for (const auto& e : akmes) {
        const int u = e.first;
        const int v = e.second;

        // varos akmis
        float w = vresApostasi(u, v) / 60.0f + 1.0f;

        // an allazeis grammi metro tote extra kostos (poini)
        if (stathmoi[u].grammi != stathmoi[v].grammi) {
            w += 2.0f;
        }

        geitones[u].push_back({ v, w });
        geitones[v].push_back({ u, w });
    }
}



bool GlobalState::vresDiadromiDijkstra(int arxiIdx, int telosIdx)
{
    const int n = (int)stathmoi.size();

    // ka8arismos proigoumenis diadromis
    exeiDiadromi = false;
    komvoiDiadromis.clear();
    akmesDiadromis.clear();

    // arxikopoihsh
    const float APEIRO = std::numeric_limits<float>::infinity();
    std::vector<float> apostaseis(n, APEIRO);
    std::vector<int> gonios(n, -1);
    std::vector<bool> episkeptitike(n, false); // Pinakas gia na kseroume ti eida

    apostaseis[arxiIdx] = 0.0f;

    // epanalipsi gia osous einai oi stathmoi
    for (int i = 0; i < n; i++) {

        // vriskoume ton stathmo me tin mikroteri apostasi 
        // apo osous den exoume episkefthei akoma
        int u = -1;
        for (int j = 0; j < n; j++) {
            if (!episkeptitike[j]) {
                if (u == -1 || apostaseis[j] < apostaseis[u]) {
                    u = j;
                }
            }
        }

        // an den vrikame stathmo h ftasame sto apeiro stop
        if (u == -1 || apostaseis[u] == APEIRO) {
            break;
        }

        // simeiwnoyme oti ton eidame
        episkeptitike[u] = true;

        // an ftasame ston stoxo teliwsame pio grigora
        if (u == telosIdx) {
            break;
        }

        
        for (auto& geitonas : geitones[u]) {
            int v = geitonas.first;
            float varos = geitonas.second;

            if (apostaseis[u] + varos < apostaseis[v]) {
                apostaseis[v] = apostaseis[u] + varos;
                gonios[v] = u;
            }
        }
    }



    // an den vrikame dromo
    if (arxiIdx != telosIdx && gonios[telosIdx] == -1) {
        return false;
    }

    // backtracking diadromis apo to telos stin arxi
    for (int cur = telosIdx; cur != -1; cur = gonios[cur]) {
        komvoiDiadromis.push_back(cur);
        if (cur == arxiIdx) break;
    }
    std::reverse(komvoiDiadromis.begin(), komvoiDiadromis.end());

    ftiakseAkmesDiadromis();
    exeiDiadromi = true;
    return true;
}




void GlobalState::ftiakseAkmesDiadromis()
{
    akmesDiadromis.clear();
    for (int i = 0; i + 1 < (int)komvoiDiadromis.size(); i++) {
        akmesDiadromis.push_back({ komvoiDiadromis[i], komvoiDiadromis[i + 1] });
    }
}




bool GlobalState::einaiAkmiDiadromis(int a, int b) const
{
    for (const auto& e : akmesDiadromis) {
        if ((e.first == a && e.second == b) ||
            (e.first == b && e.second == a))
            return true;
    }
    return false;
}




void GlobalState::elegxosPontikiou()
{
    graphics::MouseState ms;
    graphics::getMouseState(ms);

    pontikiX = (float)ms.cur_pos_x;
    pontikiY = (float)ms.cur_pos_y;

    // vriskoume stathmo p einai konta sto pontiki
    stathmosPontikiou = -1;
    float kalytero = AKTINA_KLIK_SQ;

    for (int i = 0; i < (int)stathmoi.size(); i++) {
        const float dx = pontikiX - stathmoi[i].x;
        const float dy = pontikiY - stathmoi[i].y;
        const float d2 = dx * dx + dy * dy;

        if (d2 <= kalytero) {
            kalytero = d2;
            stathmosPontikiou = i;
        }
    }



    const bool leftDown = ms.button_left_pressed;
    const bool klik = leftDown && !proigoumenoKlik;
    proigoumenoKlik = leftDown;



    if (!klik) return; // an den egine klik feugoume

    // koumpi RESET
    if (mesaStoKouti(pontikiX, pontikiY, koubibtnX, koubibtnY, koubibtnW, koubibtnH)) {
        dialegmeniArxi = -1;
        dialegmenosStoxos = -1;

        exeiDiadromi = false;
        komvoiDiadromis.clear();
        akmesDiadromis.clear();

        katastasiApp = Katastasi::EpilogiArxis;
        return;
    }


    // koumpi FIND ROUTE
    if (mesaStoKouti(pontikiX, pontikiY, koubibtnFindX, koubibtnFindY, koubibtnFindW, koubibtnFindH)) {
        if (katastasiApp == Katastasi::Etoimo) {
            exeiDiadromi = vresDiadromiDijkstra(dialegmeniArxi, dialegmenosStoxos);
        }
        return;
    }



    // epilogi stathmwn sto xarti
    if (stathmosPontikiou != -1) {
        if (katastasiApp == Katastasi::EpilogiArxis) {
            dialegmeniArxi = stathmosPontikiou;
            dialegmenosStoxos = -1;
            katastasiApp = Katastasi::EpilogiStoxou;
        }
        else if (katastasiApp == Katastasi::EpilogiStoxou) {
            dialegmenosStoxos = stathmosPontikiou;
            katastasiApp = Katastasi::Etoimo;
        }

        exeiDiadromi = false;
        komvoiDiadromis.clear();
        akmesDiadromis.clear();
    }
}




void GlobalState::zwgrafiseGrammes()
{
    graphics::Brush pinelo;
    pinelo.fill_opacity = 0.0f;
    pinelo.outline_opacity = 1.0f;

    for (const auto& e : akmes) {
        Stathmos& a = stathmoi[e.first];
        Stathmos& b = stathmoi[e.second];

        pinelo.outline_width = 5.0f;

        // xrwmatismos grammwn
        if (a.grammi != b.grammi) {
            pinelo.outline_color[0] = 0.25f; pinelo.outline_color[1] = 0.25f; pinelo.outline_color[2] = 0.25f;
        }
        else if (a.grammi == 1) { // prasini grammi
            pinelo.outline_color[0] = 0.05f; pinelo.outline_color[1] = 0.65f; pinelo.outline_color[2] = 0.10f;
        }
        else if (a.grammi == 2) { // kokkini grammi
            pinelo.outline_color[0] = 0.90f; pinelo.outline_color[1] = 0.10f; pinelo.outline_color[2] = 0.10f;
        }
        else { // mple grammi
            pinelo.outline_color[0] = 0.10f; pinelo.outline_color[1] = 0.35f; pinelo.outline_color[2] = 0.95f;
        }

        graphics::drawLine(a.x, a.y, b.x, b.y, pinelo);
    }


    // highlight tin diadromi
    if (exeiDiadromi) {
        for (const auto& e : akmesDiadromis) {
            Stathmos& a = stathmoi[e.first];
            Stathmos& b = stathmoi[e.second];

            graphics::Brush back;
            back.fill_opacity = 0.0f;
            back.outline_opacity = 1.0f;
            back.outline_width = 14.0f;
            back.outline_color[0] = 0.0f; back.outline_color[1] = 0.0f; back.outline_color[2] = 0.0f;
            graphics::drawLine(a.x, a.y, b.x, b.y, back);

            graphics::Brush top = back;
            top.outline_width = 9.0f;
            top.outline_color[0] = 1.0f; top.outline_color[1] = 0.90f; top.outline_color[2] = 0.10f; // Kitrino
            graphics::drawLine(a.x, a.y, b.x, b.y, top);
        }
    }
}




void GlobalState::zwgrafiseStathmous()
{
    graphics::Brush pinelo;
    pinelo.outline_opacity = 1.0f;
    pinelo.outline_width = 1.0f;
    pinelo.outline_color[0] = 0.0f; pinelo.outline_color[1] = 0.0f; pinelo.outline_color[2] = 0.0f;

    // elegxoume an prepei na dioume onomata
    const bool kapoioTooltip =
        (stathmosPontikiou != -1) || (dialegmeniArxi != -1) || (dialegmenosStoxos != -1);

    for (int i = 0; i < (int)stathmoi.size(); i++) {
        Stathmos& s = stathmoi[i];

        // xrwma stathmou analoga tin grammi
        if (s.grammi == 1) { pinelo.fill_color[0] = 0.20f; pinelo.fill_color[1] = 0.85f; pinelo.fill_color[2] = 0.25f; }
        else if (s.grammi == 2) { pinelo.fill_color[0] = 0.95f; pinelo.fill_color[1] = 0.25f; pinelo.fill_color[2] = 0.25f; }
        else { pinelo.fill_color[0] = 0.25f; pinelo.fill_color[1] = 0.55f; pinelo.fill_color[2] = 1.00f; }

        graphics::drawDisk(s.x, s.y, AKTINA_STATHMOU, pinelo);

        // hover efe
        if (i == stathmosPontikiou) {
            graphics::Brush ring;
            ring.fill_opacity = 0.0f;
            ring.outline_opacity = 1.0f;
            ring.outline_width = 2.0f;
            ring.outline_color[0] = 0.0f; ring.outline_color[1] = 0.0f; ring.outline_color[2] = 0.0f;
            graphics::drawDisk(s.x, s.y, DAKTYLIOS_HOVER, ring);
        }

        // start
        if (i == dialegmeniArxi) {
            graphics::Brush ring;
            ring.fill_opacity = 0.0f;
            ring.outline_opacity = 1.0f;
            ring.outline_width = 4.0f;
            ring.outline_color[0] = 0.0f; ring.outline_color[1] = 0.0f; ring.outline_color[2] = 0.0f;
            graphics::drawDisk(s.x, s.y, AKTINA_STATHMOU + 5.0f, ring);
        }

        // goal
        if (i == dialegmenosStoxos) {
            graphics::Brush ring;
            ring.fill_opacity = 0.0f;
            ring.outline_opacity = 1.0f;
            ring.outline_width = 4.0f;
            ring.outline_color[0] = 1.0f; ring.outline_color[1] = 0.9f; ring.outline_color[2] = 0.1f;
            graphics::drawDisk(s.x, s.y, AKTINA_STATHMOU + 7.0f, ring);
        }

        const bool deikseOnoma = (i == stathmosPontikiou) || (i == dialegmeniArxi) || (i == dialegmenosStoxos);
        if (deikseOnoma) {
            const float tx = s.x + 16.0f;
            const float ty = s.y - 16.0f;

            graphics::Brush kouti;
            kouti.fill_opacity = 0.85f;
            kouti.outline_opacity = 1.0f;
            kouti.outline_width = 1.0f;
            kouti.fill_color[0] = 1.0f; kouti.fill_color[1] = 1.0f; kouti.fill_color[2] = 1.0f;
            kouti.outline_color[0] = 0.0f; kouti.outline_color[1] = 0.0f; kouti.outline_color[2] = 0.0f;

            const float w = 10.0f * (float)s.onoma.size() + 14.0f;
            graphics::drawRect(tx + w / 2.0f, ty, w, 26.0f, kouti);

            graphics::Brush txt;
            txt.fill_opacity = 1.0f;
            txt.outline_opacity = 0.0f;
            txt.fill_color[0] = 0.0f; txt.fill_color[1] = 0.0f; txt.fill_color[2] = 0.0f;

            graphics::drawText(tx, ty + 6.0f, 18, s.onoma, txt);
        }

        // termatiko stathmoi exoun label
        if (!kapoioTooltip && einaiTermatikos(i)) {
            float dx = 0.0f;
            float dy = 32.0f;

            // diorthosi gia na fainontai kala
            if (s.onoma == "Piraeus (L1)") { dx = 60.0f; dy = 30.0f; }
            else if (s.onoma == "Dimotiko Theatro (L3)") { dx = -80.0f; dy = 30.0f; }

            const float tx = s.x + dx;
            const float ty = s.y + dy;

            graphics::Brush txt;
            txt.fill_opacity = 1.0f;
            txt.outline_opacity = 0.0f;
            txt.fill_color[0] = 0.0f; txt.fill_color[1] = 0.0f; txt.fill_color[2] = 0.0f;

            graphics::drawText(tx - (s.onoma.size() * 4.0f), ty, 18, s.onoma, txt);
        }
    }
}




void GlobalState::zwgrafiseUI()
{
    graphics::Brush pinelo;
    pinelo.fill_color[0] = 0.90f; pinelo.fill_color[1] = 0.90f; pinelo.fill_color[2] = 0.90f;
    pinelo.fill_opacity = 1.0f;
    pinelo.outline_opacity = 1.0f;
    pinelo.outline_width = 2.0f;
    pinelo.outline_color[0] = 0.0f; pinelo.outline_color[1] = 0.0f; pinelo.outline_color[2] = 0.0f;

    graphics::Brush txt;
    txt.fill_opacity = 1.0f;
    txt.outline_opacity = 0.0f;
    txt.fill_color[0] = 0.0f; txt.fill_color[1] = 0.0f; txt.fill_color[2] = 0.0f;

    // koumpi reset
    graphics::drawRect(koubibtnX, koubibtnY, koubibtnW, koubibtnH, pinelo);
    graphics::drawText(koubibtnX - 28, koubibtnY + 6, 20, "RESET", txt);

    // koumpi find
    graphics::drawRect(koubibtnFindX, koubibtnFindY, koubibtnFindW, koubibtnFindH, pinelo);
    graphics::drawText(koubibtnFindX - 55, koubibtnFindY + 6, 20, "FIND ROUTE", txt);

    // odigies
    graphics::drawText(30, 40, 22, "Click START station, then click GOAL station.", txt);
    if (dialegmeniArxi != -1) graphics::drawText(30, 70, 20, "Start: " + stathmoi[dialegmeniArxi].onoma, txt);
    if (dialegmenosStoxos != -1) graphics::drawText(30, 100, 20, "Goal: " + stathmoi[dialegmenosStoxos].onoma, txt);

    // lista diadromis
    if (exeiDiadromi) {
        graphics::drawText(30, 130, 20, "Route: READY (YELLOW)", txt);

        float y = 160.0f;
        std::string grammiKeimenou;
        int metritis = 0;

        for (size_t i = 0; i < komvoiDiadromis.size(); i++) {
            const Stathmos& s = stathmoi[komvoiDiadromis[i]];

            if (!grammiKeimenou.empty()) grammiKeimenou += " -> ";
            grammiKeimenou += s.onoma;
            metritis++;

            const bool teleutaio = (i == komvoiDiadromis.size() - 1);
            if (metritis == STATHMOI_ANA_GRAMMI || teleutaio) {
                graphics::drawText(30, y, 20, grammiKeimenou, txt);
                y += 22.0f;
                grammiKeimenou.clear();
                metritis = 0;
            }
        }
    }



    // ypomnima xrwmatwn
    const float lx = 980.0f;
    const float ly = 600.0f;

    graphics::Brush prasino;
    prasino.fill_opacity = 1.0f;
    prasino.outline_opacity = 0.0f;
    prasino.fill_color[0] = 0.20f; prasino.fill_color[1] = 0.85f; prasino.fill_color[2] = 0.25f;
    graphics::drawDisk(lx, ly, 8, prasino);
    graphics::drawText(lx + 15, ly + 5, 18, "Green: Metro Line 1", txt);

    graphics::Brush kokkino = prasino;
    kokkino.fill_color[0] = 0.95f; kokkino.fill_color[1] = 0.25f; kokkino.fill_color[2] = 0.25f;
    graphics::drawDisk(lx, ly + 25, 8, kokkino);
    graphics::drawText(lx + 15, ly + 30, 18, "Red: Metro Line 2", txt);

    graphics::Brush mple = prasino;
    mple.fill_color[0] = 0.25f; mple.fill_color[1] = 0.55f; mple.fill_color[2] = 1.00f;
    graphics::drawDisk(lx, ly + 50, 8, mple);
    graphics::drawText(lx + 15, ly + 55, 18, "Blue: Metro Line 3", txt);
}



bool GlobalState::einaiTermatikos(int idx) const
{
    int idioiStathmoi = 0;
    const int grammi = stathmoi[idx].grammi;

    // psaxnoume poses sindeseis exei stin idia grammi
    for (const auto& e : akmes) {
        if (e.first == idx) {
            if (stathmoi[e.second].grammi == grammi) idioiStathmoi++;
        }
        else if (e.second == idx) {
            if (stathmoi[e.first].grammi == grammi) idioiStathmoi++;
        }
    }
    // an exei 1 h ligoteres einai terma
    return idioiStathmoi <= 1;
}


std::string GlobalState::keimenoDiadromis() const
{
    if (!exeiDiadromi || komvoiDiadromis.empty()) return "-";

    std::string out;
    for (size_t i = 0; i < komvoiDiadromis.size(); i++) {
        out += stathmoi[komvoiDiadromis[i]].onoma;
        if (i + 1 < komvoiDiadromis.size()) out += " -> ";
    }
    return out;
}




void GlobalState::ftiakseXarti()
{
    stathmoi.clear();
    akmes.clear();

    // line 1 prasini
    stathmoi.push_back({ 100, "Kifissia (L1)", 1, 885,  70 });
    stathmoi.push_back({ 101, "Maroussi",      1, 835, 120 });
    stathmoi.push_back({ 102, "Irini",         1, 785, 170 });
    stathmoi.push_back({ 103, "Nea Ionia",     1, 725, 220 });
    stathmoi.push_back({ 104, "Perissos",      1, 665, 270 });
    stathmoi.push_back({ 105, "Aghios Eleftherios", 1, 605, 315 });
    stathmoi.push_back({ 106, "Attiki (L1)",   1, 530, 340 });
    stathmoi.push_back({ 107, "Omonia (L1)",   1, 600, 360 });
    stathmoi.push_back({ 108, "Monastiraki (L1)", 1, 560, 395 });
    stathmoi.push_back({ 109, "Petralona",     1, 500, 435 });
    stathmoi.push_back({ 110, "Kallithea",     1, 430, 480 });
    stathmoi.push_back({ 111, "Piraeus (L1)",  1, 300, 510 });

    // line 2 kokkini
    stathmoi.push_back({ 200, "Anthoupoli (L2)", 2, 180, 270 });
    stathmoi.push_back({ 204, "Peristeri",       2, 285, 285 });
    stathmoi.push_back({ 201, "Aghios Antonios", 2, 360, 300 });
    stathmoi.push_back({ 202, "Sepolia",         2, 450, 320 });
    stathmoi.push_back({ 203, "Attiki (L2)",     2, 530, 360 });
    stathmoi.push_back({ 205, "Omonia (L2)",     2, 600, 382 });
    stathmoi.push_back({ 206, "Panepistimio",    2, 630, 402 });
    stathmoi.push_back({ 207, "Syntagma (L2)",   2, 665, 430 });
    stathmoi.push_back({ 208, "Syngroy-Fix",     2, 670, 480 });
    stathmoi.push_back({ 209, "Aghios Ioannis",  2, 660, 545 });
    stathmoi.push_back({ 210, "Ilioupoli",       2, 645, 595 });
    stathmoi.push_back({ 211, "Elliniko (L2)",   2, 635, 655 });

    // line 3 mple
    stathmoi.push_back({ 300, "Airport (L3)",    3, 1100, 450 });
    stathmoi.push_back({ 301, "Koropi",          3,  950, 375 });
    stathmoi.push_back({ 302, "Doukissis Plakentias", 3, 925, 275 });
    stathmoi.push_back({ 303, "Halandri",        3,  860, 305 });
    stathmoi.push_back({ 304, "Katehaki",        3,  820, 325 });
    stathmoi.push_back({ 305, "Ambelokipi",      3,  750, 355 });
    stathmoi.push_back({ 306, "Evangelismos",    3,  705, 390 });
    stathmoi.push_back({ 307, "Syntagma (L3)",   3,  665, 410 });
    stathmoi.push_back({ 308, "Monastiraki (L3)",3,  545, 405 });
    stathmoi.push_back({ 309, "Kerameikos",      3,  460, 390 });
    stathmoi.push_back({ 310, "Egaleo",          3,  380, 385 });
    stathmoi.push_back({ 311, "Nikaia",          3,  310, 395 });
    stathmoi.push_back({ 312, "Piraeus (L3)",    3,  285, 485 });
    stathmoi.push_back({ 313, "Dimotiko Theatro (L3)", 3, 240, 535 });



    // voithitiki gia na vroume index apo  ID
    auto vresIndexApoId = [&](int id) -> int {
        for (int i = 0; i < (int)stathmoi.size(); i++)
            if (stathmoi[i].id == id) return i;
        return -1;
        };



    // sundesi duo stathmwn
    auto prostheseSyndesi = [&](int idA, int idB) {
        const int a = vresIndexApoId(idA);
        const int b = vresIndexApoId(idB);
        if (a != -1 && b != -1) akmes.push_back({ a, b });
        };



    // line 1
    prostheseSyndesi(100, 101); prostheseSyndesi(101, 102); prostheseSyndesi(102, 103); prostheseSyndesi(103, 104);
    prostheseSyndesi(104, 105); prostheseSyndesi(105, 106); prostheseSyndesi(106, 107); prostheseSyndesi(107, 108);
    prostheseSyndesi(108, 109); prostheseSyndesi(109, 110); prostheseSyndesi(110, 111);



    // line 2
    prostheseSyndesi(200, 204); prostheseSyndesi(204, 201); prostheseSyndesi(201, 202); prostheseSyndesi(202, 203);
    prostheseSyndesi(203, 205); prostheseSyndesi(205, 206); prostheseSyndesi(206, 207); prostheseSyndesi(207, 208);
    prostheseSyndesi(208, 209); prostheseSyndesi(209, 210); prostheseSyndesi(210, 211);




    // line 3
    prostheseSyndesi(300, 301); prostheseSyndesi(301, 302); prostheseSyndesi(302, 303); prostheseSyndesi(303, 304);
    prostheseSyndesi(304, 305); prostheseSyndesi(305, 306); prostheseSyndesi(306, 307); prostheseSyndesi(307, 308);
    prostheseSyndesi(308, 309); prostheseSyndesi(309, 310); prostheseSyndesi(310, 311);
    prostheseSyndesi(311, 312);
    prostheseSyndesi(312, 313);




    // antapokriseis (transfers)
    prostheseSyndesi(106, 203);
    prostheseSyndesi(107, 205);
    prostheseSyndesi(108, 308);
    prostheseSyndesi(207, 307);
    prostheseSyndesi(111, 312);
}