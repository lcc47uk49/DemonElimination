//
//  PlayLayer.cpp
//  DemonElimination
//
//  Created by 凯 于 on 15-6-27.
//
//

#include "PlayLayer.h"
#include "DemonFruit.h"

USING_NS_CC;
using namespace std;


Scene* PlayLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = PlayLayer::create();
    scene->addChild(layer);
    
    return scene;
}

PlayLayer::PlayLayer():m_fruitsMatrix(NULL),m_iMatrix(NULL),m_width(0),m_height(0), m_matrixLeftBottomX(0), m_matrixLeftBottomY(0),m_isTouchEnable(true), m_srcFruit(NULL),m_dstFruit(NULL),m_isAnimationing(true),m_isNeedFillVacancies(false),m_lastExplodeFruitNo(-1)
{
    m_fruitSize = Size(0, 0);
}

PlayLayer::~PlayLayer()
{
    if (m_fruitsMatrix)
    {
        free(m_fruitsMatrix);
    }
    if(m_iMatrix)
    {
        free(m_iMatrix);
    }
}

bool PlayLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }
    //随机种子
    srand((unsigned)time(NULL));
    
    Size winSize = Director::getInstance()->getWinSize();
    //适配
    //屏幕外自适应节点 --默认锚点(0,0)默认坐标屏幕左下角
    m_bgFitNode = Node::create();
    m_bgFitNode->setContentSize(Size(default_winWidth,default_winHeight));
    GameTools::getInstance()->scaleFitOutScreen(m_bgFitNode);
    this->addChild(m_bgFitNode,0);
    
    //屏幕中心自适应节点 --锚点选为底边中心点，将此节点贴着底边放置
    m_ctrFitNode = Node::create();
    m_ctrFitNode->setContentSize(Size(default_winWidth,default_winHeight));
    GameTools::getInstance()->scaleFitInScreen(m_ctrFitNode);
    m_ctrFitNode->setPosition(Point(winSize.width/2,0));
    m_ctrFitNode->setAnchorPoint(Point(0.5,0));
    this->addChild(m_ctrFitNode);
    
    //添加背景
    Sprite* spBackground = Sprite::create("background.png");
    m_bgFitNode->addChild(spBackground);
    spBackground->setPosition(Point(m_bgFitNode->getContentSize().width/2,m_bgFitNode->getContentSize().height/2));
    
    
    //加载图片资源
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("DemonFruits.plist");
    
    //为两个矩阵开辟内存
    m_width = Fruits_MATRIX_WIDTH;
    m_height = Fruits_MATRIX_HEIGHT;
    
    int arrSize = sizeof(DemonFruit*) * m_width * m_height;
    m_fruitsMatrix = (DemonFruit**)malloc(arrSize);
    memset((void*)m_fruitsMatrix, 0, arrSize);
    
    int arrSize2 = sizeof(int*) * m_width * m_height;
    m_iMatrix = (int*)malloc(arrSize2);
    memset((void*)m_iMatrix, 0, arrSize2);
    
    //初始化左下角的点的坐标和果实图片大小,这里创建的果实不addChild
    DemonFruit* fruit = DemonFruit::create(1, 1, 1);
    Rect fruitRect = fruit->boundingBox();
    m_fruitSize = fruitRect.size;
    //果实和空隙总共的宽、高
    float wid = m_width * m_fruitSize.width + (m_width -1 ) * Fruits_GAP;
//    float hgt = m_height * m_fruitSize.height + (m_height - 1) * Fruits_GAP;
    //左下角的点的横坐标为左边间隙+图片宽度的一半
    m_matrixLeftBottomX = (winSize.width - wid)/2 + m_fruitSize.width/2;
    //左下角点的纵坐标可以自己设置
    m_matrixLeftBottomY = 200;
    
    initFruitsMatrix();//根据m_iMatrix初始化果实矩阵
    
    
    //触摸绑定--单点触摸
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2(PlayLayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(PlayLayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(PlayLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    return  true;
}

void PlayLayer::initFruitsMatrix()
{
    //读取整形矩阵/生成整形矩阵
    for (int row = 0; row < m_height; row++)
    {
        for (int col = 0; col < m_width; col++)
        {
            vector<int> vecTmp = GameManager::getInstance()->getusedFruitsNo();
            
            size_t length = vecTmp.size();
            //取一个随机位置的元素值
            int randNo = rand()%length;
            m_iMatrix[row * m_width + col] = vecTmp[randNo];
        }
    }
    
    for (int row = 0; row < m_height; row++)
    {
        for (int col = 0; col < m_width; col++)
        {
            createAndDropFruits(row, col, m_iMatrix[row * m_width + col]);
        }
    }
    //开启更新函数
    scheduleUpdate();
}

void PlayLayer::createAndDropFruits(int row, int col, int index)
{
    //设置坐标
    Size size = Director::getInstance()->getWinSize();
    DemonFruit* fruit = DemonFruit::create(row, col, index);
    
    Point endPos = positionOfItem(row, col);
    Point beginPos = Point(endPos.x, endPos.y + size.height / 2);
    fruit->setPosition(beginPos);
    float speed = beginPos.y / (2 * size.height);
    fruit->runAction(MoveTo::create(speed, endPos));
    m_ctrFitNode->addChild(fruit);
    m_fruitsMatrix[row * m_width + col] = fruit;
}

Point PlayLayer::positionOfItem(int row, int col)
{
    float x = m_matrixLeftBottomX + (m_fruitSize.width + Fruits_GAP) * col;
    float y = m_matrixLeftBottomY + (m_fruitSize.height+ Fruits_GAP) * row;
    return Point(x, y);
}



bool PlayLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
    m_srcFruit = NULL;
    m_dstFruit = NULL;
    if (m_isTouchEnable)
    {
        Point localPos = m_ctrFitNode->convertTouchToNodeSpace(touch);
        m_srcFruit = fruitOfPoint(&localPos);
        //高亮显示！！
    }
    return  m_isTouchEnable;
}

void PlayLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
    if(!m_isTouchEnable || !m_srcFruit)
        return ;
    int row = m_srcFruit->getRow();
    int col = m_srcFruit->getCol();
    auto halfFruitWidth = m_fruitSize.width/2;
    auto halfFruitHeight = m_fruitSize.height/2;
    auto localPos = m_ctrFitNode->convertTouchToNodeSpace(touch);
    
    //上方区域
    auto upRect = Rect(m_srcFruit->getPositionX() - halfFruitWidth,
                       m_srcFruit->getPositionY() + halfFruitHeight + Fruits_GAP,
                       m_fruitSize.width,
                       m_fruitSize.height);
    if (upRect.containsPoint(localPos))
    {
        row++;
        if (row < m_height)
        {
            m_dstFruit = m_fruitsMatrix[row * m_width + col];
        }
        checkAndSwapFruit();
        return;
    }
   
    //下方区域
    auto downRect = Rect(m_srcFruit->getPositionX() - halfFruitWidth,
                       m_srcFruit->getPositionY() - halfFruitHeight*3 - Fruits_GAP,
                       m_fruitSize.width,
                       m_fruitSize.height);
    if (downRect.containsPoint(localPos))
    {
        row--;
        if (row >= 0)
        {
            m_dstFruit = m_fruitsMatrix[row * m_width + col];
        }
        checkAndSwapFruit();
        return;
    }
    
    //左方区域
    auto leftRect = Rect(m_srcFruit->getPositionX() - halfFruitWidth*3 - Fruits_GAP,
                         m_srcFruit->getPositionY() - halfFruitHeight,
                         m_fruitSize.width,
                         m_fruitSize.height);
    if (leftRect.containsPoint(localPos))
    {
        col--;
        if (col >= 0)
        {
            m_dstFruit = m_fruitsMatrix[row * m_width + col];
        }
        checkAndSwapFruit();
        return;
    }
    
    //右方区域
    auto rightRect = Rect(m_srcFruit->getPositionX() + halfFruitWidth + Fruits_GAP,
                         m_srcFruit->getPositionY() - halfFruitHeight,
                         m_fruitSize.width,
                         m_fruitSize.height);
    if (rightRect.containsPoint(localPos))
    {
        col++;
        if (col < m_width)
        {
            m_dstFruit = m_fruitsMatrix[row * m_width + col];
        }
        checkAndSwapFruit();
        return;
    }
    //not a useful movement
}

void PlayLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
    //去除高亮！！
    
}

DemonFruit* PlayLayer::fruitOfPoint(Point *pos)
{
    DemonFruit* fruit = NULL;
    
    for (int i = 0; i < m_height * m_width; i++)
    {
        fruit = m_fruitsMatrix[i];
        if (fruit->boundingBox().containsPoint(*pos))
        {
            CCLOG("fruit index = %d",fruit->getImgIndex());
            return fruit;
        }
    }
    return NULL;
}

void PlayLayer::checkAndSwapFruit()
{
    //交换的时候不允许触摸
    m_isTouchEnable = false;
    //设置正在执行动画
    m_isAnimationing = true;
    if (!m_srcFruit || !m_dstFruit)
    {
        return;
    }
    
    Point posOfSrc = m_srcFruit->getPosition();
    Point posOfDst = m_dstFruit->getPosition();
    float time = 0.2;
    
    //1.swap in matrix
    swapFruitInMatrix();
    
    //2. check for remove able
    std::list<DemonFruit *> colChainListOfFirst;
    getColChain(m_srcFruit, colChainListOfFirst);
    
    std::list<DemonFruit *> rowChainListOfFirst;
    getRowChain(m_srcFruit, rowChainListOfFirst);
    
    std::list<DemonFruit *> colChainListOfSecond;
    getColChain(m_dstFruit, colChainListOfSecond);
    
    std::list<DemonFruit *> rowChainListOfSecond;
    getRowChain(m_dstFruit, rowChainListOfSecond);
    
    if (colChainListOfFirst.size() >= 3
        || rowChainListOfFirst.size() >= 3
        || colChainListOfSecond.size() >= 3
        || rowChainListOfSecond.size() >= 3)
    {
        // just swap
        m_srcFruit->runAction(MoveTo::create(time, posOfDst));
        m_dstFruit->runAction(MoveTo::create(time, posOfSrc));
        return;
    }
    
    //3. no chain, swap back
    swapFruitInMatrix();
    
    m_srcFruit->runAction(Sequence::create(
                                           MoveTo::create(time, posOfDst),
                                           MoveTo::create(time, posOfSrc),
                                           NULL));
    m_dstFruit->runAction(Sequence::create(
                                           MoveTo::create(time, posOfSrc),
                                           MoveTo::create(time, posOfDst),
                                           NULL));
}

void PlayLayer::swapFruitInMatrix()//交换矩阵中的两个果实，没有改变坐标
{
    m_fruitsMatrix[m_srcFruit->getRow() * m_width + m_srcFruit->getCol()] = m_dstFruit;
    m_fruitsMatrix[m_dstFruit->getRow() * m_width + m_dstFruit->getCol()] = m_srcFruit;
    int tmpRow = m_srcFruit->getRow();
    int tmpCol = m_srcFruit->getCol();
    m_srcFruit->setRow(m_dstFruit->getRow());
    m_srcFruit->setCol(m_dstFruit->getCol());
    m_dstFruit->setRow(tmpRow);
    m_dstFruit->setCol(tmpCol);

}

//横向检查
void PlayLayer::getColChain(DemonFruit *fruit, std::list<DemonFruit *> &chainList)
{
    chainList.clear();//清理
    chainList.push_back(fruit);//将当前的果实加入
    //向左查找
    int neighborCol = fruit->getCol() - 1;
    while (neighborCol >= 0)
    {
        DemonFruit* neighborFruit = m_fruitsMatrix[fruit->getRow() * m_width + neighborCol];
        if (neighborFruit && (neighborFruit->getImgIndex() == fruit->getImgIndex())
            && fruit->getCanBeMove())//编号相同并且可以移动则加入列表
        {
            chainList.push_back(neighborFruit);
            neighborCol--;
        }
        else break;
    }
    
    //向右查找
    neighborCol = fruit->getCol() + 1;
    while(neighborCol < m_width)
    {
        DemonFruit* neighborFruit = m_fruitsMatrix[fruit->getRow() * m_width + neighborCol];
        if (neighborFruit && (neighborFruit->getImgIndex() == fruit->getImgIndex())
            && fruit->getCanBeMove())//编号相同并且可以移动则加入列表
        {
            chainList.push_back(neighborFruit);
            neighborCol++;
        }
        else break;
    }
}

//纵向
void PlayLayer::getRowChain(DemonFruit *fruit, std::list<DemonFruit *> &chainList)
{
    chainList.clear();
    chainList.push_back(fruit);
    
    //向下
    int neighborRow = fruit->getRow() - 1;
    while (neighborRow >= 0)
    {
        DemonFruit* neighborFruit = m_fruitsMatrix[neighborRow * m_width + fruit->getCol()];
        if (neighborFruit && (neighborFruit->getImgIndex() == fruit->getImgIndex())
            && fruit->getCanBeMove())//编号相同并且可以移动则加入列表
        {
            chainList.push_back(neighborFruit);
            neighborRow--;
        }
        else break;
    }
    
    //向上
    neighborRow = fruit->getRow() + 1;
    while (neighborRow < m_height)
    {
        DemonFruit* neighborFruit = m_fruitsMatrix[neighborRow * m_width + fruit->getCol()];
        if (neighborFruit && (neighborFruit->getImgIndex() == fruit->getImgIndex())
            && fruit->getCanBeMove())//编号相同并且可以移动则加入列表
        {
            chainList.push_back(neighborFruit);
            neighborRow++;
        }
        else break;
    }
}

void PlayLayer::update(float dt)
{
    //check if animationing
    if(m_isAnimationing)
    {
        //初始化为false
        m_isAnimationing = false;
        //如果有果实有动画效果则置为true
        for (int i = 0; i < m_height * m_width; i++)
        {
            DemonFruit* fruit = m_fruitsMatrix[i];
            if (fruit && (fruit->getNumberOfRunningActions() > 0))
            {
                m_isAnimationing = true;
                break;
            }
        }
    }
    
    m_isTouchEnable = !m_isAnimationing;//有动画则不能触摸
    
    if (!m_isAnimationing)
    {
        if (isDeadMap())//死地图，重新生成
        {
            m_isTouchEnable = false;//不允许触摸
            unscheduleUpdate();//关闭更新
            resetFruitMatrix();//重新生成果实矩阵
        }
        else
        {
            if (m_isNeedFillVacancies)//填充
            {
                fillVacancies();
                m_isNeedFillVacancies = false;
            }
            else//消除
            {
                checkAndRemoveChain();
            }
        }
    }
}

void PlayLayer::checkAndRemoveChain()
{
    DemonFruit* fruit = NULL;
    //遍历果实矩阵
    for (int i = 0; i < m_height * m_width; i++)
    {
        fruit = m_fruitsMatrix[i];
        if (!fruit)
        {
            continue;
        }
        if (!fruit->getCanBeMove())
        {
            continue;//不能移动的跳过检查
        }
        
        //检查并加入链表
        std::list<DemonFruit*> colChianList;
        getColChain(fruit, colChianList);
        
        std::list<DemonFruit*> rowChainList;
        getRowChain(fruit, rowChainList);
        
        if (colChianList.size() >= 3)//若有3个以上说明可以消除，标记
        {
            std::list<DemonFruit*>::iterator it = colChianList.begin();
            for (; it != colChianList.end(); it++)
            {
                DemonFruit* fruitTmp = *it;
                fruitTmp->setIsNeedRemove(true);
            }
        }
        if (rowChainList.size() >= 3)//若有3个以上说明可以消除，标记
        {
            std::list<DemonFruit*>::iterator it = rowChainList.begin();
            for (; it != rowChainList.end(); it++)
            {
                DemonFruit* fruitTmp = *it;
                fruitTmp->setIsNeedRemove(true);
            }
        }
    }
    removeFruit();
}

void PlayLayer::removeFruit()
{
    m_isAnimationing = true;//标记正在执行动画
    //遍历消除果实矩阵
    for(int i = 0; i < m_height * m_width; i++)
    {
        DemonFruit* fruit = m_fruitsMatrix[i];
        if (!fruit)
        {
            continue;
        }
        
        if (fruit->getIsNeedRemove())
        {
            m_isNeedFillVacancies = true;//需要掉落
            //消除----这里用多态么？fruit->explode，子类实现爆炸效果，火焰果实燃烧，冰系果实破冰...
            int rowNo = fruit->getRow();
            int colNo = fruit->getCol();
            fruit->explodeFruit();//爆破移除
            m_fruitsMatrix[rowNo * m_width + colNo] = NULL;//置为空
        }
    }
}

void PlayLayer::fillVacancies()
{
    m_isAnimationing = true;
    
    Size size = Director::getInstance()->getWinSize();
    int *colEmptyInfo = (int *)malloc(sizeof(int) * m_width);
    memset((void*)colEmptyInfo, 0, sizeof(int) * m_width);
    
    //1.drop exist Fruit
    DemonFruit* fruit = NULL;
    for (int col = 0; col < m_width; col++)
    {
        int removedFruitOfCol = 0;
        //from buttom to top
        for (int row = 0; row < m_height; row++)
        {
            fruit = m_fruitsMatrix[row * m_width + col];
            if (NULL == fruit)
            {
                removedFruitOfCol++;//记录一列中空缺的果实个数
            }
            else
            {
                if (removedFruitOfCol > 0)
                {
                    int newRow = row - removedFruitOfCol;//计算果实将掉落到的行数
                    //掉落果实
                    m_fruitsMatrix[newRow * m_width + col] = fruit;
                    m_fruitsMatrix[row * m_width + col] = NULL;
                    //移动到新位置
                    Point startPos = fruit->getPosition();
                    Point endPos = positionOfItem(newRow, col);
                    float speed = (startPos.y - endPos.y)/size.height;
                    fruit->stopAllActions();//停止其他动作
                    fruit->runAction(MoveTo::create(speed, endPos));
                    
                    fruit->setRow(newRow);//设置果实新行
                }
            }
        }
        colEmptyInfo[col] = removedFruitOfCol;//记录此列空缺的个数
    }
    
    //2.create new item and drop down
    for (int col = 0; col < m_width; col++)
    {
        for (int row = m_height - colEmptyInfo[col]; row < m_height; row++)
        {
            //从选择使用的果实中随机选择一种
            vector<int> vecTmp = GameManager::getInstance()->getusedFruitsNo();
            
            size_t length = vecTmp.size();
            //取一个随机位置的元素值
            int randNo = rand()%length;
            int randIndex = vecTmp[randNo];
            createAndDropFruits(row, col, randIndex);
        }
    }
    
    free(colEmptyInfo);
}

bool PlayLayer::isDeadMap()
{
    //遍历m_fruitsMatrix
    for (int row = 0; row < m_height; row++)
    {
        for (int col = 0; col < m_width; col++)
        {
            //检查上方
            if (row + 2 >= m_height - 1)//防止上方越界
            {
                //do noting;
            }
            else
            {
                DemonFruit* curFruit = m_fruitsMatrix[row * m_width + col];
                DemonFruit* fruit1 = m_fruitsMatrix[(row+2) * m_width + col];
                DemonFruit* fruit2 = m_fruitsMatrix[(row+3) * m_width + col];
                if (NULL == curFruit || NULL == fruit1 || NULL == fruit2)
                {
                    //如果为空，说明正在生成，认为不是死地图，下一帧在检查
                    return false;
                }
                int curIndex = curFruit->getImgIndex();
                int upIndex1 = fruit1->getImgIndex();//当前上方第二个果实编号
                int upIndex2 = fruit2->getImgIndex();//当前上方第三个果实编号
                if (curIndex == upIndex1 && curIndex == upIndex2)
                {
                    return false;//如果上方2、3与自己标记相同，说明可以消除，不是死地图
                }
            }
            
            //检查下方
            if (row - 2 <= 0)//防止下方越界
            {
                //do noting;
            }
            else
            {
                DemonFruit* curFruit = m_fruitsMatrix[row * m_width + col];
                DemonFruit* fruit1 = m_fruitsMatrix[(row-2) * m_width + col];
                DemonFruit* fruit2 = m_fruitsMatrix[(row-3) * m_width + col];
                if (NULL == curFruit || NULL == fruit1 || NULL == fruit2)
                {
                    //如果为空，说明正在生成，认为不是死地图，下一帧在检查
                    return false;
                }
                int curIndex = curFruit->getImgIndex();//当前果实的编号
                int downIndex1 = fruit1->getImgIndex();
                int downIndex2 = fruit2->getImgIndex();
                if (curIndex == downIndex1 && curIndex == downIndex2)
                {
                    return false;//可以消除，不是死地图
                }
            }
            
            //检查左方
            if (col - 2 <= 0)//防止左方越界
            {
                //do noting;
            }
            else
            {
                DemonFruit* curFruit = m_fruitsMatrix[row * m_width + col];
                DemonFruit* fruit1 = m_fruitsMatrix[row * m_width + col - 2];
                DemonFruit* fruit2 = m_fruitsMatrix[row * m_width + col - 3];
                if (NULL == curFruit || NULL == fruit1 || NULL == fruit2)
                {
                    //如果为空，说明正在生成，认为不是死地图，下一帧在检查
                    return false;
                }
                int curIndex = curFruit->getImgIndex();//当前果实的编号
                int leftIndex1 = fruit1->getImgIndex();
                int leftIndex2 = fruit2->getImgIndex();
                
                if (curIndex == leftIndex1 && curIndex == leftIndex2)
                {
                    return false;//可以消除，不是死地图
                }
            }
            
            //检查右方
            if (col + 2 >= m_width - 1)//防止右方越界
            {
                //do noting;
            }
            else
            {
                DemonFruit* curFruit = m_fruitsMatrix[row * m_width + col];
                DemonFruit* fruit1 = m_fruitsMatrix[row * m_width + col + 2];
                DemonFruit* fruit2 = m_fruitsMatrix[row * m_width + col + 3];
                if (NULL == curFruit || NULL == fruit1 || NULL == fruit2)
                {
                    //如果为空，说明正在生成，认为不是死地图，下一帧在检查
                    return false;
                }
                int curIndex = curFruit->getImgIndex();//当前果实的编号
                int rightIndex1 = fruit1->getImgIndex();
                int rightIndex2 = fruit2->getImgIndex();
                
                if (curIndex == rightIndex1 && curIndex == rightIndex2)
                {
                    return false;//可以消除，不是死地图
                }
            }
        }
    }
    CCLOG("Dead Map!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    return true;//死地图
}

void PlayLayer::resetFruitMatrix()
{
    m_isAnimationing = true;//重置时有动画
    float dt = 0.05;
    //当前的果实全部变成灰色
    for (int row = 0; row < m_height; row++)
    {
        for (int col = 0; col < m_width; col++)
        {
            DemonFruit* fruit = m_fruitsMatrix[row * m_width + col];
            auto tt = TintTo::create(dt, 163, 163, 163);
//            //这里果实矩阵和果实图片均是从左下角向右上角依次变灰，加个蒙版。
//            //fruitTurnGray(fruit);
            DelayTime* delayTime = DelayTime::create(dt * (row+col));
            fruit->runAction(Sequence::create(delayTime,tt,CallFuncN::create(CC_CALLBACK_1(PlayLayer::actionEndCallback, this)), NULL));
        }
    }
    //重新生成果实矩阵
    auto callBack = CallFunc::create(CC_CALLBACK_0(PlayLayer::initFruitsMatrix, this));
    Sequence* sq = Sequence::create(DelayTime::create(dt * (m_width+m_height)),callBack,NULL);
    this->runAction(sq);

}


void PlayLayer::actionEndCallback(Node* node)
{
    DemonFruit* fruit = dynamic_cast<DemonFruit*>(node);
    fruit->removeFromParent();
}
