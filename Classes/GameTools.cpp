//
//  GameTools.cpp
//  DemonElimination
//
//  Created by 凯 于 on 15-6-30.
//
//

#include "GameTools.h"


//singleton stuff
static  GameTools* s_SharedGameTools = nullptr;

GameTools* GameTools::getInstance()
{
    if ( !s_SharedGameTools)
    {
        s_SharedGameTools = new GameTools();
    }
    return s_SharedGameTools;
}

GameTools::GameTools()
{
    initObj();
}

GameTools::~GameTools()
{
    
}

void GameTools::initObj()
{
    //实际屏幕尺寸
    m_winSize = Director::getInstance()->getWinSize();
    //设计屏幕尺寸，往往是背景图片大小
    m_defaultSize = Size(default_winWidth, default_winHeight);
    
    //获取缩放比例
    m_fScaleX       = m_winSize.width / m_defaultSize.width;
    m_fScaleY       = m_winSize.height / m_defaultSize.height;
    
    m_fWinRatio     = m_winSize.width / m_winSize.height;
    m_fDefaultRatio = m_defaultSize.width / m_defaultSize.height;
    
    m_fScaleFit        = m_fWinRatio > m_fDefaultRatio ? m_fScaleY : m_fScaleX;
    m_fScaleFitReverse = m_fWinRatio > m_fDefaultRatio ? m_fScaleX : m_fScaleY;
}


//全屏拉伸，可能形变
void GameTools::scaleFullScreen(Node *_node)
{
    _node->setScaleX(this->m_fScaleX);
    _node->setScaleY(this->m_fScaleY);
}

//不出屏幕
void GameTools::scaleFitInScreen(Node *_node)
{
    _node->setScale(this->m_fScaleFit);
}

//超出屏幕
void GameTools::scaleFitOutScreen(Node *_node)
{
    _node->setScale(this->m_fScaleFitReverse);
}

void GameTools::scaleXScreen(Node *_node){
    _node->setScale(this->m_fScaleX);
}

void GameTools::scaleYScreen(Node *_node)
{
    _node->setScale(this->m_fScaleY);
}

std::string GameTools::createBufFormat(const char *fmt, ...)
{
    char buffer[1024];
    
    va_list argptr;
    va_start(argptr, fmt);
    CCAssert(vsprintf(buffer, fmt, argptr) != -1,"vsprintf cross the border！！");
    va_end(argptr);
    return std::string(buffer);
}
