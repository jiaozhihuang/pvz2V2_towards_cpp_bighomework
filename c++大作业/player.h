#pragma once
#include "camera.h"
#include <graphics.h>
#include "vector2.h"
#include "player_id.h"
#include "animation.h"
#include "platform.h"
#include "bullet.h"
#include "particle.h"
extern Atlas atlas_run_effect; 
extern Atlas atlas_jump_effect;
extern Atlas atlas_land_effect;

extern IMAGE img_1P_cursor;
extern IMAGE img_2P_cursor;


extern bool is_debug;

extern std::vector<Bullet*>bullet_list;
extern std::vector<Platform>platform_list;
class Player
{
public:
	Player(bool facing_right = true) : is_facing_right(facing_right)
	{
		current_animation = is_facing_right ? &animation_idle_right : &animation_idle_left;

		timer_attack_cd.set_wait_time(attack_cd);
		timer_attack_cd.set_one_shot(true);
		timer_attack_cd.set_callback([&]()
			{
				can_attack = true;
			});

		timer_invulnerable.set_wait_time(750);
		timer_invulnerable.set_one_shot(true);
		timer_invulnerable.set_callback([&]()
			{
				is_invulnerable = false;
			});

		timer_invulnerable_blink.set_wait_time(75);
		timer_invulnerable_blink.set_callback([&]()
			{
				is_showing_sketch_frame = !is_showing_sketch_frame;

			});

		timer_run_effect_generation.set_wait_time(75);
		timer_run_effect_generation.set_callback([&]()
			{
				Vector2 particle_position;
				IMAGE* frame = atlas_run_effect.get_image(0);
				//将粒子对象放置在玩家脚底处
				particle_position.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_position.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(particle_position, &atlas_run_effect, 45);
			});

		timer_die_effect_generation.set_wait_time(35);
		timer_die_effect_generation.set_callback([&]()
			{
				Vector2 particle_position;
				IMAGE* frame = atlas_run_effect.get_image(0);
				particle_position.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_position.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(particle_position, &atlas_run_effect, 150);

			});
		animation_jump_effect.set_atlas(&atlas_jump_effect);
		animation_jump_effect.set_interval(25);
		animation_jump_effect.set_loop(false);
		animation_jump_effect.set_callback([&]()
			{
				is_jump_effect_visible = false;
			});

		animation_land_effect.set_atlas(&atlas_land_effect);
		animation_land_effect.set_interval(50);
		animation_land_effect.set_loop(false);
		animation_land_effect.set_callback([&]()
			{
				is_land_effect_visible = false;
			});

		timer_cursor_visibility.set_wait_time(2500);
		timer_cursor_visibility.set_one_shot(true);
		timer_cursor_visibility.set_callback([&]()
			{
				is_cursor_visible = false;
			});


	}
	~Player() = default;

	void set_hp(int val)
	{
		hp = val;
	}

	int get_hp()
	{
		return hp;
	}

	int get_mp()
	{
		return mp;
	}
	void set_position(float x,float y)
	{
		position.x = x; position.y = y;
	}

	const Vector2& get_position()const
	{
		return position;
	}

	const Vector2& get_size()const
	{
		return size;
	}

	virtual void on_run(float distance)
	{   
		if (is_attacking_ex)
			return;

		position.x += distance;

		timer_run_effect_generation.resume();
	}

	virtual void on_jump()
	{
		if (velocity.y != 0 || is_attacking_ex)
			return;

		velocity.y += jump_velocity;
		is_jump_effect_visible = true;
		animation_jump_effect.reset();

		IMAGE* effect_frame = animation_jump_effect.get_frame();
		position_jump_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_jump_effect.y = position.y + size.y - effect_frame->getheight();

	}

	virtual void on_land()
	{
		is_land_effect_visible = true;
		animation_land_effect.reset();

		IMAGE* effect_frame = animation_land_effect.get_frame();
		position_land_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_land_effect.y = position.y + size.y - effect_frame->getheight();
	}

	virtual void on_update(int delta)
	{
		int direction = is_right_key_down - is_left_key_down;

		if (direction != 0)
		{   
			if(!is_attacking_ex)
			   is_facing_right = direction > 0;
			current_animation = is_facing_right ? &animation_run_right : &animation_run_left;
			float distance = direction * run_velocity * delta;
			on_run(distance);

		}
		else
		{
			current_animation = is_facing_right ? &animation_idle_right : &animation_idle_left;
			timer_run_effect_generation.pause();
		}
		if (is_attacking_ex)
			current_animation = is_facing_right ? &animation_attack_ex_right : &animation_attack_ex_left;

		if (hp <= 0)
			current_animation = last_hurt_direction.x < 0 ? &animation_die_left : &animation_die_right;


		current_animation->on_update(delta);
		animation_jump_effect.on_update(delta);
		animation_land_effect.on_update(delta);

		timer_attack_cd.on_update(delta);
		timer_invulnerable.on_update(delta);
		timer_invulnerable_blink.on_update(delta);
		timer_run_effect_generation.on_update(delta);
		timer_cursor_visibility.on_update(delta);

		if (hp <= 0)
			timer_die_effect_generation.on_update(delta);

		particle_list.erase(std::remove_if(
			particle_list.begin(), particle_list.end(),
			[](const Particle& particle)
				{
					return !particle.check_valid();

				}),
			particle_list.end());
		for (Particle& particle : particle_list)
			particle.on_update(delta);
		

		if (is_showing_sketch_frame)
			sketch_image(current_animation->get_frame(), &img_sketch);

		move_and_collide(delta);
	}

	virtual void on_draw(const Camera& camera)
	{   
		if (is_jump_effect_visible)
			animation_jump_effect.on_draw(camera, (int)position_jump_effect.x, (int)position_jump_effect.y);

		if (is_land_effect_visible)
			animation_land_effect.on_draw(camera, (int)position_land_effect.x, (int)position_land_effect.y);

		for ( Particle& particle : particle_list)
			particle.on_draw(camera);

		if (hp > 0 && is_invulnerable && is_showing_sketch_frame)
			putimage_alpha(camera, (int)position.x, (int)position.y, &img_sketch);
		else
		    current_animation->on_draw(camera, (int)position.x, (int)position.y);

		if (is_cursor_visible)
		{
			switch (id)
			{
			case PlayerID::P1:
				putimage_alpha(camera, (int)(position.x + (size.x - img_1P_cursor.getwidth()) / 2),
					(int)(position.y - img_1P_cursor.getheight()), &img_1P_cursor); break;
			case PlayerID::P2:
				putimage_alpha(camera, (int)(position.x + (size.x - img_2P_cursor.getwidth()) / 2),
					(int)(position.y - img_2P_cursor.getheight()), &img_2P_cursor); break;
			}
		}
		if (is_debug)
		{
			setlinecolor(RGB(0, 125, 255));
			rectangle((int)position.x, (int)position.y, (int)(position.x + size.x), (int)(position.y + size.y));
		}
	}

	void make_invulnerable()
	{
		is_invulnerable = true;
		timer_invulnerable.restart();
	}

	virtual void on_input(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (id)
			{
			case PlayerID::P1:
				switch (msg.vkcode)
				//0xXX包括后面的VK_XX都指的是按键的ASCII码表示
				{
				case 0x41:is_left_key_down = true; break;
				case 0x44:is_right_key_down = true; break;
				case 0x57:on_jump(); break;
				case 0x46:
					if (can_attack)
					{
						on_attack();
						can_attack = false;
						timer_attack_cd.restart();
					}
					break;
				case 0x47:
					if (mp >= 100)
					{
						on_attack_ex();
						mp = 0;
					}
					break;
				}
				break;
			case PlayerID::P2:
				switch (msg.vkcode)
				{
				case VK_LEFT:is_left_key_down = true; break;
				case VK_RIGHT:is_right_key_down = true; break;
				case VK_UP:on_jump(); break;
				case 0x4E:
					if (can_attack)
					{
						on_attack();
						can_attack = false;
						timer_attack_cd.restart();
					}
					break;
				case 0x4D:
					if (mp >= 100)
					{
						on_attack_ex();
						mp = 0;
					}
					break;
				}
				break;
			}
			break;
		case WM_KEYUP:
			switch (id)
			{
			case PlayerID::P1:
				switch (msg.vkcode)
				{
				case 0x41:is_left_key_down = false; break;
				case 0x44:is_right_key_down = false; break;
				}
				break;
			case PlayerID::P2:
				switch (msg.vkcode)
				{
				case VK_LEFT:is_left_key_down = false; break;
				case VK_RIGHT:is_right_key_down = false; break;
				}
				break;
		    }
		 break;
	    }

	}
	void set_id(PlayerID id)
	{
		this->id = id;
	}

	virtual void on_attack(){  }
	virtual void on_attack_ex(){  }
protected:
	int mp = 0;      //能量条
	int hp = 100;    //血条

	Vector2 position;    //角色位置
	Vector2 size;        //玩家尺寸
	Vector2 velocity;    //玩家速度

	Animation animation_idle_left;    //朝左默认动画
	Animation animation_idle_right;   //朝右默认动画
	Animation animation_run_left;
	Animation animation_run_right;
	Animation animation_attack_ex_left;  //向左特殊攻击动画
	Animation animation_attack_ex_right; //向右特殊攻击动画

	Animation animation_jump_effect;
	Animation animation_land_effect;
	Animation animation_die_left;
	Animation animation_die_right;

	bool is_jump_effect_visible = false;
	bool is_land_effect_visible = false;

	Vector2 position_jump_effect;
	Vector2 position_land_effect;

	Animation* current_animation = nullptr;  //当前正在播放的动画

	PlayerID id = PlayerID::P1;

	bool is_left_key_down = false;    //按键是否按下
	bool is_right_key_down = false;

	bool is_facing_right = true;    //角色是否朝右

	int attack_cd = 500;
	bool can_attack = true;    //是否可以普通攻击（普攻有间隔）
	Timer timer_attack_cd;    //普攻冷却计时器

	bool is_attacking_ex = false;  //是否在释放特殊攻击

	IMAGE img_sketch;     //动画帧剪影图片
	bool is_invulnerable = false;    //角色是否处于无敌
	bool is_showing_sketch_frame = false;    //当前帧是否应当显示剪影，作用是标志无敌效果

	Timer timer_invulnerable;        //无敌状态计时器
	Timer timer_invulnerable_blink;  //无敌状态闪烁计时器

	std::vector<Particle>particle_list;  //粒子对象存放处

	Timer timer_run_effect_generation;  //跑动粒子特效计时器
	Timer timer_die_effect_generation;  //死亡粒子特效计时器

	bool is_cursor_visible = true;    //光标
	Timer timer_cursor_visibility;

	Vector2 last_hurt_direction;  //最后一次受击方向


protected:
	const float run_velocity = 0.55f;    //跑动速度
	const float gravity = 1.6e-3f;       //玩家重力
	const float jump_velocity = -0.85f;  //跳跃速度

protected:
	void move_and_collide(int delta)
	{   
		float last_velocity_y = velocity.y;

		velocity.y += gravity * delta;
		position += velocity * (float)delta;

		if (hp <= 0)
			return;

		if (velocity.y > 0)
		{
			for (const Platform& platform : platform_list)
			{
				const Platform::CollisionShape& shape = platform.shape;

				//水平碰撞判断，两者碰撞等价于两者图形的碰撞，为方便处理我们均视为矩形
				//水平碰撞条件为两者的最右端点与最左端点做差小于了两者水平方向上长度和，即两者水平方向上有重合部分
				bool is_collide_x = (max(position.x + size.x, shape.right) - min(position.x, shape.left)
					<= size.x + (shape.right - shape.left));

				//竖直碰撞判断
                //竖直碰撞条件与水平碰撞类似，也是通过判断两者是否有重合，不过竖直方向上的碰撞要求在竖直和水平方向上图形均有重合
				bool is_collide_y = (shape.y >= position.y && shape.y <= position.y + size.y);

				if (is_collide_x && is_collide_y)
				{
					float delta_pos_y = velocity.y * delta;
					float last_tick_foot_pos_y = position.y + size.y - delta_pos_y;
					if (last_tick_foot_pos_y <= shape.y)    //eazyx库坐标原点在左上角，向右/下方向坐标值增加
					{
						position.y = shape.y - size.y;    //碰撞后重新设置玩家位置，速度
						velocity.y = 0;

						//落地前竖直速度不为零而落地后速度为0时触发落地动画逻辑
						if (last_velocity_y != 0)
							on_land();
						break;
					}

				}
			}
		}
		//子弹与玩家的碰撞逻辑
		if (!is_invulnerable)
		{
			for (Bullet* bullet : bullet_list)
			{
				if (!bullet->get_valid() || bullet->get_collide_target() != id)//检查子弹是否有效以及碰撞的玩家id
					continue;

				if (bullet->check_collision(position, size))
				{
					make_invulnerable();
					bullet->on_collide();
					bullet->set_valid(false);
					hp -= bullet->get_damage();
					last_hurt_direction = bullet->get_position() - position;
					if (hp <= 0)
					{
						velocity.x = last_hurt_direction.x < 0 ? 0.35f : -0.35f;
						velocity.y = -1.0f;
					}
				}
			}
		}
	}


		



};