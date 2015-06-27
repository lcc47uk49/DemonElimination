//
//  PlayLayer.cpp
//  DemonElimination
//
//  Created by 凯 于 on 15-6-27.
//
//

#include "PlayLayer.h"
USING_NS_CC;

Scene* PlayLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = PlayLayer::create();
    scene->addChild(layer);
    
    return scene;
}

bool PlayLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }
    Size winSize = Director::getInstance()->getWinSize();
    auto layerColor = LayerColor::create(Color4B(232, 233, 232, 255), winSize.width, winSize.height);
    this->addChild(layerColor);
    return  true;
}