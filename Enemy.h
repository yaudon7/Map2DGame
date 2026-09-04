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
	int hImage_;//画像ID
	Point pos_;//位置
	DIR dir_;//移動方向
public:
	Enemy();
	~Enemy();
	void Update() override;
	void PatrolUpdate();
	void ChaseUpdate();
	void AttackUpdate();
	void SearchUpdate();
	void Draw() override;
	bool IsAttackRange();
	bool CheckPlayerInSight();//Playerが視界に入ったかの関数
private:
	bool isFindPlayer_;//プレイヤーを見つけたか
	float patrol_timer;//enemyの向いている方向の変更時間
	float chase_timer;//playerを追跡する移動の間隔時間
	float search_timer;//捜索する方向の変更時間
	float search_end_timer;//patrolに戻る時間
	PANDA_STATE state_;//enemyの状態
};

