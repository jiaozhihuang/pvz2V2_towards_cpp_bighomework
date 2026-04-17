#pragma once
#include <graphics.h>
#include <vector>

class Atlas
{
public:
	Atlas() = default;
	~Atlas() = default;

	void load_from_file(LPCTSTR path_template, int num)//第一个参数是常指针，指向固定位置的图片
	{
		img_list.clear();//清空图片对象列表
		img_list.resize(num);//将列表长度设置为指定大小
		TCHAR path_file[256];//TCHAR表示字符类型，与字符编码环境无关，便于移植
		for (int i = 0; i < num; i++)

		{
			//将图片路径字符串拼接到字符数组path_file字符数组中，并在其后添加i+1得到完整路径
			_stprintf_s(path_file, path_template, i + 1);
			loadimage(&img_list[i], path_file);
		}

	}
	void clear()
	{
		img_list.clear();
	}
	int get_size()//获取图片（成员）尺寸 ,size是vector库里的函数
	{
		return (int)img_list.size();
	}
	IMAGE* get_image(int idx)
	{
		if (idx < 0 || idx >= img_list.size())
			return nullptr;

		return &img_list[idx];
	}
	void add_image(const IMAGE& img)//添加新的图片对象
	{
		img_list.push_back(img);
	}

private:
	std::vector<IMAGE> img_list;
 
};
