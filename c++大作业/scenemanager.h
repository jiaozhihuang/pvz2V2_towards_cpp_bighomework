#pragma once
#include "scene.h"
#include "camera.h"
//enxtern关键字获得源文件中两个指针的引用
extern Scene* menu_scene;
extern Scene* game_scene;
extern Scene* selector_scene;
class SceneManager
{
public:
	enum class SceneType //设置枚举类型表示游戏场景状态
	{
		Menu,Game,Selector
	};
public:
	SceneManager() = default;
	~SceneManager() = default;
	void set_current_scene(Scene* scene)//该函数功能设置当前场景
	{
		current_scene = scene; current_scene->on_enter();
	}
	void switch_to(SceneType type)//跳转场景
	{
		current_scene->on_exit();
		switch (type)
		{
		case SceneType::Menu:current_scene = menu_scene; break;
		case SceneType::Game:current_scene = game_scene; break;
		case SceneType::Selector:current_scene = selector_scene; break;
		default:break;
		}
		current_scene->on_enter();

	}
	void on_update(int delta) { current_scene->on_update(delta); }
	void on_draw(const Camera&camera)
	{
		current_scene->on_draw(camera); 
	}
	void on_input(const ExMessage& msg) { current_scene->on_input(msg); }
private:
	Scene* current_scene = nullptr;
};
