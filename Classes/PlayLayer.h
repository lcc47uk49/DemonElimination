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
#include "DemonFruit.h"
#include "GameManager.h"
#include "GameTools.h"
using namespace cocos2d;

#define Fruits_MATRIX_WIDTH (10)
#define Fruits_MATRIX_HEIGHT (10)
//间隙
#define Fruits_GAP  (6)
class PlayLayer : public Layer
{
public:
    PlayLayer();
    ~PlayLayer();
    
    static Scene* createScene();
    virtual bool init();
    CREATE_FUNC(PlayLayer);
    //单点触摸
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    virtual void update(float dt);
private:
    //根据传入的整形矩阵初始化果实矩阵
    void initFruitsMatrix();
    //根据行列号和编号创建果实
    void createAndDropFruits(int row, int col, int index);
    //返回矩阵元素对应的坐标
    Point positionOfItem(int row, int col);
    //根据点击的位置选择对应的果实
    DemonFruit* fruitOfPoint(Point* pos);
    //交换两个果实
    void swapFruit();
    //将与fruit相同编号的果实加入队列chainList
    void getColChain(DemonFruit *fruit, std::list<DemonFruit *> &chainList);
    void getRowChain(DemonFruit *fruit, std::list<DemonFruit *> &chainList);
    
    void checkAndRemoveChain();
    void removeFruit();//消除被标记的果实
    void fillVacancies();//下落填充空缺
private:
    DemonFruit** m_fruitsMatrix;//果实矩阵
    int m_iMatrix[Fruits_MATRIX_WIDTH][Fruits_MATRIX_HEIGHT];//整形矩阵
    int m_width;//列数
    int m_height;//行数
    float m_matrixLeftBottomX;//矩阵做下角的点的横坐标
    float m_matrixLeftBottomY;//矩阵做下角的点的纵坐标
    Size m_fruitSize;//果实大小 64 X 64
    int m_lastExplodeFruitNo;//最后一次消除的果实的编号
    
    Node *m_bgFitNode;//背景图屏幕外自适应
    Node *m_ctrFitNode;//屏幕内中心自适应
    DemonFruit* m_srcFruit;//玩家移动的果实
    DemonFruit* m_dstFruit;//移动到那个果实
    
    bool m_isTouchEnable;//是否可以触摸
    bool m_isAnimationing;//是否正在执行动画
    bool m_isNeedFillVacancies;//是否有空缺需要填补
    
};
#endif /* defined(__DemonElimination__PlayLayer__) */
