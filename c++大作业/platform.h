#pragma once
#include "camera.h"
#include "util.h"

extern bool is_debug;
class Platform
{
public:
	Platform() = default;
	~Platform() = default;
public:
	struct CollisionShape  //平台碰撞外形
	{
		float y;  //高度
		float left, right;  //水平位置，长度
	};
public:
	CollisionShape shape;
	IMAGE* img = nullptr;  //渲染图片
	POINT render_position = { 0 };  //渲染位置


	void on_draw(const Camera& camera)const  //平台自身绘图逻辑
	{
		putimage_alpha(camera, render_position.x, render_position.y, img);
		
		if (is_debug)
		{
			setlinecolor(RGB(255, 0, 0));
			line(camera, (int)shape.left, (int)shape.y, (int)shape.right, (int)shape.y);
		}


	}
};
