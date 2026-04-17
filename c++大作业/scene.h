#pragma once
#include <graphics.h>
#include "camera.h"
class Scene
{public:
	Scene() = default;
	~Scene() = default;
	//成员函数定义为虚函数
	// 方便具体的游戏场景继承基类Scene公有成员后重写自己的逻辑时保持接口的一致性
	virtual void on_enter() { }//场景开始
	virtual void on_update(int delta){ }//数据更新
	virtual void on_draw(const Camera& camera){ }// 场景渲染
	virtual void on_input(const ExMessage& msg ){ }//用户输入
	virtual void on_exit(){ }//场景离开||退出

 private:



};
