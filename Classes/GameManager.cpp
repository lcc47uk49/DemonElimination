//
//  GameManager.cpp
//  DemonElimination
//
//  Created by 凯 于 on 15-6-29.
//
//

#include "GameManager.h"
//最多选择的恶魔果实个数
#define TOTAL_SELECTEDFRUIT_NUM (7)

//singleton stuff
static GameManager *s_SharedManager = nullptr;

//所有的果实名字，前4种为普通果实，后面的全为恶魔果实
static const string s_allFruitsArr[] = {"f0.png","f1.png","f2.png","f3.png","f4.png","f5.png","f6.png"};

GameManager* GameManager::getInstance()
{
    if ( !s_SharedManager)
    {
        s_SharedManager = new GameManager();
    }
    return s_SharedManager;
}

GameManager::GameManager()
{
    //初始化，默认解锁、选择4、5、6三种恶魔果实
    m_unlockFruitsNo.push_back(4);
    m_unlockFruitsNo.push_back(5);
    m_unlockFruitsNo.push_back(6);
    
    m_selectedFruitsNo.push_back(4);
    m_selectedFruitsNo.push_back(5);
    m_selectedFruitsNo.push_back(6);
    
    //初始化所用的所有果实为0~6七种
    for (int i = 0; i < TOTAL_SELECTEDFRUIT_NUM; i++)
    {
        m_usedFruitsNo.push_back(i);
        string strTmp = s_allFruitsArr[i];
        m_usedFruitsName.push_back(strTmp);
    }
}

GameManager::~GameManager()
{
    
}

//添加一个解锁的恶魔果实的编号
void GameManager::addunlockFruitsNo(const int a)
{
    //不能重复
    vector<int>::iterator it = m_unlockFruitsNo.begin();
    for (; it != m_unlockFruitsNo.end(); it++)
    {
        int tmp = *it;
        if (tmp == a)
        {
            CCLOG("alread unlocked!");
            return;
        }
    }
    m_unlockFruitsNo.push_back(a);
}

//返回已解锁的恶魔果实编号的引用
vector<int>& GameManager::getunlockFruitsNo()
{
    return m_unlockFruitsNo;
}

//添加一个选择的恶魔果实的编号
void GameManager::addselectedFruitsNo(const int a)
{
    //最多选择个数
    if(m_selectedFruitsNo.size() == TOTAL_SELECTEDFRUIT_NUM)
        return;
    //不能重复
    vector<int>::iterator it = m_selectedFruitsNo.begin();
    for (; it != m_selectedFruitsNo.end(); it++)
    {
        int tmp = *it;
        if (tmp == a)
        {
            CCLOG("alread selected!");
            return;
        }
    }
    m_selectedFruitsNo.push_back(a);
}

//移除一个选择的恶魔果实的编号
void GameManager::rmvselectedFruitsNo(const int a)
{
    //最少选择个数
    if (m_selectedFruitsNo.size() <= 0)
    {
        return;
    }
    vector<int>::iterator it = m_selectedFruitsNo.begin();
    vector<int>::iterator rmv = m_selectedFruitsNo.begin();
    for (; it != m_selectedFruitsNo.end(); it++)
    {
        int tmp = *it;
        if (tmp == a)
        {
            rmv = it;
            break;
        }
    }
    m_selectedFruitsNo.erase(rmv);
}
//返回已选择的恶魔果实编号的引用
vector<int>& GameManager::getselectedFruitsNo()
{
    return m_selectedFruitsNo;
}

//设置将要使用的果实的编号
void GameManager::setusedFruitsNo()
{
    //先清空
    m_usedFruitsNo.clear();
    //用所有选择的恶魔果实进行游戏
    if(m_selectedFruitsNo.size() == TOTAL_SELECTEDFRUIT_NUM)
    {
        vector<int>::iterator it = m_selectedFruitsNo.begin();
        for (; it != m_selectedFruitsNo.end(); it++)
        {
            int tmp = *it;
            m_usedFruitsNo.push_back(tmp);
        }
    }
    //使用选择的恶魔果实和部分非恶魔果实
    else if (m_selectedFruitsNo.size() < TOTAL_SELECTEDFRUIT_NUM && m_selectedFruitsNo.size() >= 3)
    {
        //非恶魔果实个数
        size_t k = TOTAL_SELECTEDFRUIT_NUM - m_selectedFruitsNo.size();
        for (int i = 0; i < k; i++)
        {
            m_usedFruitsNo.push_back(i);
        }
        //恶魔果实
        vector<int>::iterator it = m_selectedFruitsNo.begin();
        for (; it != m_selectedFruitsNo.end(); it++)
        {
            int tmp = *it;
            m_usedFruitsNo.push_back(tmp);
        }
    }
}

//返回将要使用的果实的编号
vector<int>& GameManager::getusedFruitsNo()
{
    return m_usedFruitsNo;
}


//根据已解锁的和已选择的编号，设置将要使用的恶魔果实的名字
void GameManager::setusedFruitsName()
{
    //先清空！！！
    m_usedFruitsName.clear();
    vector<int>::iterator it = m_usedFruitsNo.begin();
    for (; it != m_usedFruitsNo.end(); it++)
    {
        int tmp = *it;
        string strTmp = s_allFruitsArr[tmp];
        m_usedFruitsName.push_back(strTmp);
    }
}
//返回将要是用的恶魔果实名字的引用
vector<string>& GameManager::getusedFruitsName()
{
    return m_usedFruitsName;
}