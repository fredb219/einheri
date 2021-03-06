/*
 * FastWorldDrawer.cpp
 *
 *  Created on: 5 sept. 2010
 *      Author: fred
 */

#include "FastWorldDrawer.h"
#include <math.h>
#include <SFML/Graphics.hpp>
#include <einheri/utils/Log.h>
#include <einheri/utils/FileFinder.h>

using einUtils::FileFinder;

namespace ein {

FastWorldDrawer::FastWorldDrawer(ClientGameManager *manager) :
    manager(manager) {

}

FastWorldDrawer::~FastWorldDrawer() {

}

void FastWorldDrawer::Init() {

    // Set color and depth clear value
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 0.f);

    // Disable Z-buffer for 2d
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    monsterListIndex = glGenLists(1);

    // compile the monster display list, store a triangle in it
    glNewList(monsterListIndex, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    glColor3f(0, 1, 1);
    glVertex3f(0.5f, 0.f, 0.f);
    glColor3f(0, 1, 0);
    glVertex3f(-0.25f, 0.43f, 0.f);
    glVertex3f(-0.25f, -0.43f, 0.f);
    glEnd();
    glEndList();

    
    sf::Image marineImage;
        if (!marineImage.LoadFromFile(FileFinder::get().file("share/marine.png")))
            return;

    glGenTextures(1, &marineTextureId);
    glBindTexture(GL_TEXTURE_2D, marineTextureId);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, marineImage.GetWidth(), marineImage.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, marineImage.GetPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_COLOR_MATERIAL);

    glAlphaFunc(GL_GREATER, 0.5f);
    glShadeModel(GL_SMOOTH);
    //glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);

    bloodRenderer.Init();
}

void FastWorldDrawer::Draw() {

    clearView();

    configureCamera();

    paintGround();
    paintDecorations();
    paintCorpses();
    paintProjectiles();
    paintMonsters();
    paintHeroes();

    paintEffects();

    paintCursor();

}

void FastWorldDrawer::Resize(Vector newWindowSize) {

    windowSize = newWindowSize;

    glViewport(0, 0, windowSize.getX(), windowSize.getY());

    double h = 1;

    if (windowSize.getY() != 0) {
        h = windowSize.getX() / windowSize.getY();
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-(manager->GetCameraModel()->GetDistance() * h) / 2., (manager->GetCameraModel()->GetDistance() * h) / 2., -manager->GetCameraModel()->GetDistance() / 2.,
            manager->GetCameraModel()->GetDistance() / 2.);

}

//Private

void FastWorldDrawer::clearView() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FastWorldDrawer::configureCamera() {

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vector cameraPosition = manager->GetCameraModel()->GetPosition();
    glTranslated(-cameraPosition.getX(), -cameraPosition.getY(), 0);
}

void FastWorldDrawer::paintGround() {

    float size = 100;
    
    glBegin(GL_QUADS);
    glColor3f(0.7, 0.8, 0.7);
    glVertex3f(-size, -size, 0.f);
    glVertex3f(size, -size, 0.f);
    glVertex3f(size, size, 0.f);
    glVertex3f(-size, size, 0.f);


    glEnd();

    // Draw line grid
    glPushMatrix();
    glTranslated(-size, -size,0);

    glBegin(GL_LINES);
    glColor3f(0.6, 0.8, 0.6);
        
    for(int i = 0; i< size*2; i++) {
        // Horizontal line    
        glVertex3f(0, i, 0.f);
        glVertex3f(size*2, i, 0.f);
        
        // Vertical line
        glVertex3f(i, 0, 0.f);
        glVertex3f(i, size*2, 0.f);

    }

    glEnd();
    glPopMatrix();
    
}
void FastWorldDrawer::paintCorpses() {

}
void FastWorldDrawer::paintDecorations() {
    std::list<Decoration *>::const_iterator it;

        for (it = manager->GetDecorationModel()->GetDecorations().begin(); it != manager->GetDecorationModel()->GetDecorations().end(); ++it) {
            Decoration *decoration = *it;
            bloodRenderer.Render(decoration);
        }
}

void FastWorldDrawer::paintMonsters() {

    std::list<Monster *>::const_iterator it;

    for (it = manager->GetModel()->GetMonsters().begin(); it != manager->GetModel()->GetMonsters().end(); ++it) {

        glPushMatrix();
        Monster *monster = *it;
        glTranslatef(monster->GetPosition().getX(), monster->GetPosition().getY(), 0);

        glRotatef(monster->GetAngle() * 180 / M_PI, 0, 0, 1);

        glCallList(monsterListIndex);

        glPopMatrix();
    }

}

void FastWorldDrawer::paintHeroes() {

    std::list<Hero *>::const_iterator it;

    for (it = manager->GetModel()->GetHeroes().begin(); it != manager->GetModel()->GetHeroes().end(); it++) {
        glPushMatrix();
        Hero *hero = *it;
        glTranslatef(hero->GetPosition().getX(), hero->GetPosition().getY(), 0);
        glRotatef(30+hero->GetAngle() * 180 / M_PI, 0, 0, 1);
        glTranslatef(0.0, -0.1, 0);
        float size = 0.5;
        float texOffset = 1;

            glBindTexture( GL_TEXTURE_2D, marineTextureId );
            glEnable(GL_TEXTURE_2D);


            glBegin(GL_QUADS);
            glColor3f(1, 1, 1);
            glTexCoord2f(texOffset,0);
            glVertex3f(-size, -size, 0.f);
            glTexCoord2f(0,0);
            glVertex3f(size, -size, 0.f);
            glTexCoord2f(0,texOffset);
            glVertex3f(size, size, 0.f);
            glTexCoord2f(texOffset,texOffset);
            glVertex3f(-size, size, 0.f);


            glEnd();

            glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }

}
void FastWorldDrawer::paintProjectiles() {

    std::list<Projectile *>::const_iterator it;

    for (it = manager->GetModel()->GetProjectiles().begin(); it != manager->GetModel()->GetProjectiles().end(); ++it) {
        glPushMatrix();
        Projectile *projectile = *it;
        glTranslatef(projectile->GetPosition().getX(), projectile->GetPosition().getY(), 0);

        glRotatef(projectile->GetAngle() * 180 / M_PI, 0, 0, 1);

        glBegin(GL_TRIANGLES);
        glColor3f(1, 1, 1);
        glVertex3f(-0.545f, 0.f, 0.f);
        glColor4f(1, 1, 1, 0.0);
        glVertex3f(0.0225f, 0.020f, 0.f);
        glVertex3f(0.0225f, -0.020f, 0.f);
        glEnd();

        glPopMatrix();
    }

}

void FastWorldDrawer::paintEffects() {

}

void FastWorldDrawer::paintCursor() {

    Vector mousePoint = manager->GetInputModel()->GetMouse();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, manager->GetCameraModel()->GetWindowSize().getX(), manager->GetCameraModel()->GetWindowSize().getY(), 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    int length = 20;
    int thickness = 3;

    glTranslatef(mousePoint.getX(), mousePoint.getY(), 0);
    glBegin(GL_TRIANGLES);
    glColor4f(1, 0, 0, 0.8);
    glVertex3f(-thickness, length, 0.f);
    glVertex3f(thickness, length, 0.f);
    glColor4f(1, 1, 0, 0.5);
    glVertex3f(0, 0, 0.f);

    glColor4f(1, 0, 0, 0.8);
    glVertex3f(-thickness, -length, 0.f);
    glVertex3f(thickness, -length, 0.f);
    glColor4f(1, 1, 0, 0.5);
    glVertex3f(0, 0, 0.f);

    glColor4f(1, 0, 0, 0.8);
    glVertex3f(length, thickness, 0.f);
    glVertex3f(length, -thickness, 0.f);
    glColor4f(1, 1, 0, 0.5);
    glVertex3f(0, 0, 0.f);

    glColor4f(1, 0, 0, 0.8);
    glVertex3f(-length, thickness, 0.f);
    glVertex3f(-length, -thickness, 0.f);
    glColor4f(1, 1, 0, 0.5);
    glVertex3f(0, 0, 0.f);

    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

}

}
