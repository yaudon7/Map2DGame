#include "Enemy.h"
#include "time.h"
#include "Stage.h"
#include "Player.h"

namespace
{
	const int ENEMY_SIZE = 48; //敵のサイズ 32*32
	const Point ENEMY_START_POS = { 20 * ENEMY_SIZE, 10 * ENEMY_SIZE }; //敵の初期位置
	const DIR INIT_ENEMY_DIR = { LEFT };
	const int ENEMY_DRAW_SIZE = 32; //敵の描画サイズ
	const int animFrame[4]{ 0, 1, 2, 1 };
	const float ANIM_INTERVAL = 0.2f;
}


Enemy::Enemy()
	: GameObject()
{
	hImage_ = LoadGraph("Assets/panda_R.png");
	pos_ = ENEMY_START_POS; //32はブロックの位置pos_
	dir_ = INIT_ENEMY_DIR;
	state_ = Patrol;
	isFindPlayer_ = false;
	isAttackRange_ = false;
	patrol_timer = 3.0f;
	chase_timer = 0.0f;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	switch (state_)
	{
	case Patrol:
		PatrolUpdate();
		break;
	case Chase:
		ChaseUpdate();
	}


	//GetRand(数値)
	//3秒に1回向きをランダムに変える
	/*static float dir_timer = 3.0f;
	static float prog_timer = 0.5f;
	float dt = Time::DeltaTime();
	dir_timer = dir_timer - dt;
	prog_timer = prog_timer - dt;*/


	/*	Point newPos = pos_;*/

		/*if (abs(diff.x) > abs(diff.y))
		{
			dir_ = (diff.x > 0) ? RIGHT : LEFT;
		}
		else
		{
			dir_ = (diff.y > 0) ? DOWN : UP;
		}

		if (prog_timer < 0.0f && isFindPlayer_)
		{
			switch (dir_)
			{
			case UP:
				newPos.y -= ENEMY_DRAW_SIZE;
				break;
			case DOWN:
				newPos.y += ENEMY_DRAW_SIZE;
				break;
			case LEFT:
				newPos.x -= ENEMY_DRAW_SIZE;
				break;
			case RIGHT:
				newPos.x += ENEMY_DRAW_SIZE;
				break;
			default:
				break;
			}*/

			//int mapValue = FindGameObject<Stage>()->GetMap(newPos.x / CHA_SIZE, newPos.y / CHA_SIZE);

			////移動先がステージの外に出ないようにする->壁じゃないなら移動
			//if (mapValue != 1)
			//{
			//	pos_ = newPos;
			//}
			//prog_timer = 0.5f + prog_timer;

}

void Enemy::PatrolUpdate()
{
	patrol_timer = patrol_timer - Time::DeltaTime();

	if (patrol_timer <= 0.0f) {
		switch (dir_)
		{
		case UP:
			pos_.y -= ENEMY_DRAW_SIZE;
			break;
		case DOWN:
			pos_.y += ENEMY_DRAW_SIZE;
			break;
		case LEFT:
			pos_.x -= ENEMY_DRAW_SIZE;
			break;
		case RIGHT:
			pos_.x += ENEMY_DRAW_SIZE;
			break;
		default:
			break;
		}
	}

	if (patrol_timer <= 0.0f) {
		dir_ = (DIR)GetRand(MAX_DIR);;
		patrol_timer = 3.0f;
	}

	//playerが視界に入ったら
	if (CheckPlayerInSight())
	{
		isFindPlayer_ = true;
		state_ = Chase;
		return;
	}
}

void Enemy::ChaseUpdate()
{
	if (CheckPlayerInSight() == false)
	{
		isFindPlayer_ = false;
		state_ = Patrol;
		return;
	}

	if (IsAttackRange())
	{
		state_ = Attack;
		return;
	}

	//タイマーを減らす
	chase_timer -= Time::DeltaTime();
	//プレイヤーの位置を取得
	Point playerPos = FindGameObject<Player>()->GetPlayerPos();
	//EnemyからPlayerへの方向
	Point diff = { playerPos.x - pos_.x, playerPos.y - pos_.y };

	if (abs(diff.x) > abs(diff.y))
	{
		dir_ = (diff.x > 0) ? RIGHT : LEFT;
	}
	else
	{
		dir_ = (diff.y > 0) ? DOWN : UP;
	}

	if (chase_timer <= 0.0f)
	{
		switch (dir_)
		{
		case UP:
			pos_.y -= ENEMY_DRAW_SIZE;
			break;
		case DOWN:
			pos_.y += ENEMY_DRAW_SIZE;
			break;
		case LEFT:
			pos_.x -= ENEMY_DRAW_SIZE;
			break;
		case RIGHT:
			pos_.x += ENEMY_DRAW_SIZE;
			break;
		default:
			break;
		}

		chase_timer = 0.5f;
	}
}


void Enemy::Draw()
{
	static float animTimer = ANIM_INTERVAL;
	static int frame = 0;
	int nowFrame = animFrame[frame];

	Rect iRect[4] = {
		{  nowFrame * ENEMY_SIZE, 3 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 0 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 1 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE},
		{  nowFrame * ENEMY_SIZE, 2 * ENEMY_SIZE, ENEMY_SIZE, ENEMY_SIZE}
	};
	DrawBox(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		GetColor(255, 255, 0), FALSE, 2);
	DrawRectExtendGraph(pos_.x, pos_.y, pos_.x + ENEMY_DRAW_SIZE, pos_.y + ENEMY_DRAW_SIZE,
		iRect[dir_].x, iRect[dir_].y, iRect[dir_].w, iRect[dir_].h, hImage_, TRUE);
	if (animTimer < 0) {
		frame = (++frame) % 4;
		animTimer = ANIM_INTERVAL + animTimer;
	}
	animTimer = animTimer - Time::DeltaTime();
}

bool Enemy::CheckPlayerInSight()
{
	//プレイヤーの位置を取得
	Point playerPos = FindGameObject<Player>()->GetPlayerPos();
	//EnemyからPlayerへの方向
	Point diff = { playerPos.x - pos_.x, playerPos.y - pos_.y };
	//Playerまでの距離
	float dis = sqrt(diff.x * diff.x + diff.y * diff.y);
	//視界の距離
	float viewDis = ENEMY_SIZE * 5;
	//視界外ならfalse
	if (dis > viewDis || dis <= 0.0f)
	{
		return false;
	}
	//敵の正面方向
	Point forward;
	switch (dir_)
	{
	case UP:
		forward = { 0,-1 };
		break;
	case DOWN:
		forward = { 0,1 };
		break;
	case LEFT:
		forward = { -1,0 };
		break;
	case RIGHT:
		forward = { 1,0 };
		break;
	default:
		return false;
	}
	//playerへの方向を正規化
	float playerDirX = diff.x / dis;
	float playerDirY = diff.y / dis;
	//内積
	float dot = forward.x * playerDirX +
		forward.y * playerDirY;
	//角度を求める
	float angle = acos(dot) * 180.0f / 3.14159265f;
	//視界の範囲
	float viewAngle = 90.0f;
	//視界内ならtrue
	if (angle <= viewAngle)
	{
		return true;
	}
	return false;
}

bool Enemy::IsAttackRange()
{
	//プレイヤーの位置を取得
	Point playerPos = FindGameObject<Player>()->GetPlayerPos();
	//EnemyからPlayerへの方向
	Point diff = { playerPos.x - pos_.x, playerPos.y - pos_.y };
	//Playerまでの距離
	float dis = sqrt(diff.x * diff.x + diff.y * diff.y);
	//視界の距離
	float attackDis = ENEMY_SIZE;
	//視界外ならfalse
	if (dis > attackDis || dis <= 0.0f)
	{
		return false;
	}
	
	if (attackDis >= dis)
	{
		return true;
	}
	return false;
}
