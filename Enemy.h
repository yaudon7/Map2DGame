#pragma once
#include ".\Library\GameObject.h"
#include "global.h"


enum PANDA_STATE
{
	Patrol,
	Chase,
	Attack,
	Search
};

class Enemy :
    public GameObject
{
	int hImage_;//‰æ‘œID
	Point pos_;//ˆÊ’u
	DIR dir_;//ˆÚ“®•ûŒü
public:
	Enemy();
	~Enemy();
	void Update() override;
	void PatrolUpdate();
	void Draw() override;
private:
	bool isFindPlayer_;
	bool isAttackRange_;
	PANDA_STATE state_;
};

