#include "MyPG.h"
#include "MyGameMain.h"

namespace Title
{
	
	//ゲーム情報
	DG::Image::SP imgBG;

	void Initialize()
	{
		ge->dgi->EffectState().param.bgColor = ML::Color(0, 0, 0, 0);
		imgBG = DG::Image::Create("./data/image/titleBG.png");
	}
	void  Finalize()
	{
		imgBG.reset();
	}
    TaskFlag  UpDate()
	{
		auto inp = ge->in1->GetState();
		TaskFlag rtv = TaskFlag::Title;//取りあえず現在のタスクを指
		
		if (true == inp.SE.down) {
			rtv = TaskFlag::Game;
		}
		return rtv;
	}
	void  Render()
	{
		ML::Box2D draw(0, 0, 480, 280);
		ML::Box2D src(0, 0, 480, 280);
		imgBG->Draw(draw, src);

	}

}