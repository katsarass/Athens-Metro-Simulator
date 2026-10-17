#include "sgg/graphics.h"
#include "GlobalState.h"

// wrapper sunartisi gia draw pou zitaei i SGG
void zwgrafise()
{
    GlobalState::getInstance().draw();
}

// wrapper sunartisi gia update
void enimerose(float ms)
{
    GlobalState::getInstance().update(ms);
}

int main()
{
    
    const int platos = 1200;
    const int ypsos = 700;

    
    graphics::createWindow(platos, ypsos, "Athens Metro Simulator");

    
    graphics::setCanvasSize((float)platos, (float)ypsos);
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);

    
    // an den tin vrei psaxnoume sta windows
    if (!graphics::setFont("assets/segoeuib.ttf")) {
        graphics::setFont("C:\\Windows\\Fonts\\segoeuib.ttf");
    }
    


    // kaloume to init tou Singleton gia na ftiaksei ton xarti
    GlobalState::getInstance().init();



    // dinoume tis dikes mas sunartiseis stin vivliothiki
    graphics::setDrawFunction(zwgrafise);
    graphics::setUpdateFunction(enimerose);



    // ksekinaei to loop tou paixnidiou
    graphics::startMessageLoop();

    return 0;
}