/*
 * EngineGraphic.cpp
 *
 *  Created on: 4 sept. 2010
 *      Author: fred
 */

#include "EngineGraphic.h"
#include <einheri/common/Event.h>
#include <einheri/common/event/EventWindowClose.h>
#include <einheri/common/GameManager.h>
#include <einheri/utils/FileFinder.h>
#include <sstream>
#include "einheri/common/event/EventWindowCreated.h"
#include <einheri/common/event/EventVisitor.h>
#include "WorldDrawer.h"
#include "FastWorldDrawer.h"

using einUtils::FileFinder;

namespace ein {

EngineGraphic::EngineGraphic(ClientGameManager* manager) :
    FramerateEngine(manager) {
    clientManager = manager;
    renderWindow = NULL;
    UseGameTime(false);

    worldDrawer = new WorldDrawer(manager);
}

EngineGraphic::~EngineGraphic() {
    delete renderWindow;
    delete worldDrawer;
}

void EngineGraphic::Apply(const Event& event) {
    class EngineGraphicVisitor: public EventVisitor {
    public:
        EngineGraphicVisitor(EngineGraphic* engine) :
            engine(engine) {
        }

        void Visit(const EventWindowResized& evenWindowResized) {
            engine->worldDrawer->Resize(evenWindowResized.getSize());
        }
    private:
        EngineGraphic* engine;
    };
    EngineGraphicVisitor visitor(this);
    event.accept(visitor);
}

void EngineGraphic::UseFastRender(bool fastRender){
    delete worldDrawer;

    if(fastRender) {
        worldDrawer = new FastWorldDrawer(clientManager);
    } else {
        worldDrawer = new WorldDrawer(clientManager);
    }
}

void EngineGraphic::frame(EinValue) {
    if (renderWindow == NULL) {
        initWindow();
        initGlContext();
    } else {
        if (renderWindow->IsOpened()) {
            paint();
        } else {
            manager->AddEvent(new EventWindowClose());
        }
    }
}

void EngineGraphic::initWindow() {
    // Create the main window
    sf::WindowSettings Settings;
    Settings.DepthBits = 24; // Request a 24 bits depth buffer
    Settings.StencilBits = 8; // Request a 8 bits stencil buffer
    Settings.AntialiasingLevel = 8; // Request 2 levels of antialiasing
    renderWindow = new sf::RenderWindow(sf::VideoMode(1280, 1024, 32), "Einheri", sf::Style::Close | sf::Style::Resize, Settings);
    renderWindow->ShowMouseCursor(false);
    renderWindow->PreserveOpenGLStates(true);
    clientManager->GetCameraModel()->SetWindowSize(Vector(renderWindow->GetWidth(), renderWindow->GetHeight()));

    // Create a graphical string to display
    if (!font.LoadFromFile(FileFinder::get().file("share/DejaVuSans.ttf")))
        return;
    title = sf::String("Einheri", font, 16);
    fps = sf::String("0 fps", font, 16);
    fps.SetPosition(0, 20);

    fpsCount = 0;
    framerateClock.Reset();

    worldDrawer->Init();
    worldDrawer->Resize(Vector(renderWindow->GetWidth(), renderWindow->GetHeight()));

    manager->AddEvent(new EventWindowCreated(renderWindow));

}

void EngineGraphic::initGlContext() {
    // Set color and depth clear value
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    // Disable Z-buffer for 2d
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    //zoom = 20;

    //Resize(application->app->GetWidth(), application->app->GetHeight());

    /**/

}
void EngineGraphic::paint() {

    fpsCount++;

    worldDrawer->Draw();

    renderWindow->Draw(title);

    if (framerateClock.GetElapsedTime() > 1) {
        float framerate = fpsCount / framerateClock.GetElapsedTime();
        framerateClock.Reset();
        fpsCount = 0;
        std::ostringstream oss;
        oss.precision(1);
        oss.setf(std::ios::fixed);
        oss << framerate;
        fps.SetText(oss.str() + " fps");
        fps.SetPosition(0, 20);
    }

    renderWindow->Draw(fps);

    renderWindow->Display();
}

}
