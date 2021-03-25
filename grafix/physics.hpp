#pragma once
#include "polygon.hpp"
#include "collision.hpp"
#include "vector2.hpp"

#include <array>

void physicsUpdate(std::vector<Polygon*> entityPs, double pdt) {
    // collision handeling
    for (size_t i = 0; i < entityPs.size(); i++) {
        for (size_t j = i+1; j < entityPs.size(); j++) {
            if (!(entityPs[i]->immovable*entityPs[i]->imrotatable*entityPs[j]->immovable*entityPs[j]->imrotatable)) {
                if (entityPs[i]->hitbox.collides(entityPs[j]->hitbox)) {
                    CollisionData collisionData = isColliding(*entityPs[i], *entityPs[j]);
                    if (collisionData.colliding) {
                        Polygon* left = collisionData.leftPoly;
                        Polygon* right = collisionData.rightPoly;
                        DVec2 collisionVector = collisionData.collisionVector;
                        DVec2 leftCollisionVector = collisionData.collisionPoint - left->mid;
                        DVec2 rightCollisionVector = collisionData.collisionPoint - right->mid;

                        DVec2 leftTranslationVelocity = left->vel;
                        DVec2 leftRotationVelocity = left->rotVel*leftCollisionVector.getOrthogonal();
                        DVec2 leftVelocity = leftTranslationVelocity + leftRotationVelocity;

                        DVec2 rightTranslationVelocity = right->vel;
                        DVec2 rightRotationVelocity = right->rotVel*rightCollisionVector.getOrthogonal();
                        DVec2 rightVelocity = rightTranslationVelocity + rightRotationVelocity;
                        DVec2 collisionVelocity = leftVelocity - rightVelocity;

                        // spring force
                        double k = 10000;
                        DVec2 springForce = -k*collisionVector*collisionData.collisionDepth;
                
                        // damping force
                        double d = 80;
                        double speed = collisionVelocity.dot(collisionVector);
                        DVec2 dampingForce = -d*collisionVector*speed;

                        // friction force
                        DVec2 frictionForce(0, 0);
                        if (collisionVelocity.getSquaredLength() != 0) {
                            double mu = 0.5;
                            frictionForce = -mu*collisionVector.getLength()*collisionVelocity/collisionVelocity.getLength();
                        }

                        DVec2 totalForce = springForce + dampingForce + frictionForce;

                        // translation
                        left->force += totalForce;
                        right->force += -totalForce;

                        // rotation
                        left->tourqe += leftCollisionVector.cross(totalForce);
                        right->tourqe += rightCollisionVector.cross(-totalForce);
                    }
                }
            }
        }
    }
            
    // air resistance
    for (Polygon* entity: entityPs) {
        // translation drag
        DVec2 vel = entity->vel;
        if (vel.getSquaredLength() != 0) {
            double Cd = 0.25;
            double lineArea = entity->getLength(vel.getNormalized().getOrthogonal());
            DVec2 dragForce = -Cd*lineArea*vel;
            entity->force += dragForce;
        }

        // rotation drag
        double Cd = 0.15;
        double rotVel = entity->rotVel;
        double radius = entity->radius;
        entity->tourqe += -2.0/3*Cd*rotVel*radius*radius*radius;
    }

    // update position and velocity
    for (Polygon* entity: entityPs) {
        entity->update(pdt);
    }
}