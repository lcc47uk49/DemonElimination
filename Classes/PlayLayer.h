//
//  PlayLayer.h
//  DemonElimination
//
//  Created by 凯 于 on 15-6-27.
//
//

#ifndef __DemonElimination__PlayLayer__
#define __DemonElimination__PlayLayer__

#include <stdio.h>
#include "cocos2d.h"
using namespace cocos2d;

class PlayLayer : public Layer
{
public:
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(PlayLayer);
};
#endif /* defined(__DemonElimination__PlayLayer__) */
