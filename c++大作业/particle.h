#pragma once
#include "atlas.h"
#include "vector2.h"
#include "util.h"




class Particle
{
public:
	Particle() = default;
	Particle(const Vector2& position,Atlas* atlas,int lifespan)
		:position(position),atlas(atlas),lifespan(lifespan){ }
	~Particle() = default;

	void set_atlas(Atlas* new_atlas)
	{
		atlas = new_atlas;
	}

	void set_position(const Vector2& new_position)
	{
		position = new_position;
	}
	
	void set_lifespan(int ms)
	{
		lifespan = ms;
	}

	bool check_valid()const
	{
		return valid;
	}

	void on_update(int delta)
	{
		timer += delta;
		if (timer >= delta)
		{
			timer = 0;
			idx_frame++;
			if (idx_frame >= atlas->get_size())
			{
				idx_frame = atlas->get_size() - 1;
				valid = false;
			}
		}

	}

	void on_draw(const Camera& camera)
	{
		putimage_alpha(camera, (int)position.x, (int)position.y, atlas->get_image(idx_frame));
	}

private:
	int timer = 0;    //粒子动画播放计时器
	int lifespan = 0; //单帧粒子动画持续时长
	int idx_frame = 0;//当前播放的动画帧 
	Vector2 position;
	bool valid = true;
	Atlas* atlas = nullptr;


};
