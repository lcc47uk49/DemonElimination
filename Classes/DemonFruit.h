//
//  DemonFruit.h
//  DemonElimination
//
//  Created by 凯 于 on 15-6-27.
//
//  64 X 64

#ifndef __DemonElimination__DemonFruit__
#define __DemonElimination__DemonFruit__

#include <stdio.h>
#include "cocos2d.h"
USING_NS_CC;

class DemonFruit : public Sprite
{
public:
    DemonFruit();
    //根据行列以及果实编号创建
    static DemonFruit* create(int row, int col, int index);
    void explodeFruit();//消除并移除
    void actionEndCallback(Node* node);//消除回调函数
    
    CC_SYNTHESIZE(int, m_row, Row);
    CC_SYNTHESIZE(int, m_col, Col);
    CC_SYNTHESIZE(int, m_imgIndex, ImgIndex);
    CC_SYNTHESIZE(bool, m_isNeedRemove, IsNeedRemove);//是否将要消除，初始化为false
    CC_SYNTHESIZE(bool, m_canBeMove, CanBeMove);//是否可以移动，初始化为true
};
#endif /* defined(__DemonElimination__DemonFruit__) */