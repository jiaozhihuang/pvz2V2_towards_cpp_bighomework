#pragma once
#include "timer.h"
#include "scene.h"
#include "scenemanager.h"
#include "animation.h"
#include <iostream>
#include "camera.h"
extern IMAGE img_menu_background;

extern SceneManager scene_manager;

class MenuScene :public Scene

{
public:
	MenuScene() = default;
	~MenuScene() = default;
	//基类Scene成员函数定义为虚函数，在派生类中同名，被重写覆盖
	//派生类的成员函数前virtual可去
	void on_enter() //场景开始
	{
		mciSendString(_T("play bgm_menu repeat from 0"), NULL, 0, NULL);
	}
	void on_draw(const Camera& camera) // 场景画面渲染
	{   
		putimage(0, 0, &img_menu_background);
	}
	void on_input(const ExMessage& msg) //用户输入以切换场景
	{
		if (msg.message == WM_KEYUP)//松下案件场景跳转
		{
			mciSendString(_T("play ui_confirm from 0"), NULL, 0, NULL);
			scene_manager.switch_to(SceneManager::SceneType::Selector);
		}
		
	}
	
private:
	

};