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
	dir_ = INIT_ENEMY_DIR;//方向の初期化
	state_ = Patrol;
	isFindPlayer_ = false;
	patrol_timer = 3.0f;
	chase_timer = 0.0f;
	search_timer = 1.0f;
	search_end_timer = 4.0f;
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
		break;
	case Attack:
		AttackUpdate();
		break;
	case Search:
		SearchUpdate();
		break;
	}

	Point newPos = pos_;
	int mapValue = FindGameObject<Stage>()->GetMap(newPos.x / CHA_SIZE, newPos.y / CHA_SIZE);
	//移動先がステージの外に出ないようにする->壁じゃないなら移動
	if (mapValue != 1)
	{
		pos_ = newPos;
	}
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
		dir_ = (DIR)GetRand(3);
		patrol_timer = 3.0f;
	}

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

	chase_timer -= Time::DeltaTime();
	Point playerPos = FindGameObject<Player>()->GetPlayerPos();
	//EnemyからPlayerへの方向
	Point diff = { playerPos.x - pos_.x, playerPos.y - pos_.y };
	//Playerのいる方向によってEnemyの方向を決める
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

void Enemy::AttackUpdate()
{
	DrawFormatString(40, 40, GetColor(255, 0, 0), "Attack!!");
	if (IsAttackRange() == false)
	{
		search_end_timer = 4.0f;
		state_ = Search;
		return;
	}
}

void Enemy::SearchUpdate()
{
	search_timer -= Time::DeltaTime();
	search_end_timer -= Time::DeltaTime();

	if (search_timer <= 0.0f)
	{
		dir_ = (DIR)((dir_ + 1) % 4);

		search_timer = 1.0f;
	}

	if (IsAttackRange())
	{
		state_ = Attack;
		return;
	}

	if (!CheckPlayerInSight() && search_end_timer <= 0.0f)
	{
		state_ = Patrol;
		return;
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
	float viewDis = ENEMY_SIZE * 7;
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
	float viewAngle = 60.0f;
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
	float attackDis = ENEMY_SIZE * 2;
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
