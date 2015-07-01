//
//  DemonFruit.cpp
//  DemonElimination
//
//  Created by 凯 于 on 15-6-27.
//
//

#include "DemonFruit.h"
#include "GameManager.h"

DemonFruit::DemonFruit()
{
    m_row = -1;
    m_col = -1;
    m_imgIndex = -1;
    m_isNeedRemove = false;
    m_canBeMove = true;
}

DemonFruit* DemonFruit::create(int row, int col, int index)
{
    DemonFruit* fruit = new DemonFruit();
    fruit->m_row = row;
    fruit->m_col = col;
    fruit->m_imgIndex = index;
    vector<string> fruitsNameArr = GameManager::getInstance()->getusedFruitsName();
    fruit->initWithSpriteFrameName(fruitsNameArr[index]);
    fruit->autorelease();
    
    //缩放到64 X 64
    Size contentSize = fruit->getContentSize();
    float scale = 64 / contentSize.width;
    fruit->setScale(scale);
    
    return fruit;
}

void DemonFruit::explodeFruit()
{
    float time = 0.3;
    Point pos = this->getPosition();
    int index = this->getImgIndex();
    this->runAction(Sequence::create(
                                     ScaleTo::create(time, 0.0),
                                     CallFuncN::create(CC_CALLBACK_1(DemonFruit::actionEndCallback, this)),
   
                                     NULL));
    //消除特效根据果实的不同而不同
//    Effect::getInstance()->explodeEffect(pos,index);
}

void DemonFruit::actionEndCallback(cocos2d::Node *node)
{
    DemonFruit* fruit = dynamic_cast<DemonFruit*>(node);
    fruit->removeFromParent();
}

