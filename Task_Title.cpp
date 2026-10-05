#include "MyPG.h"
#include "MyGameMain.h"

namespace Title
{
	
	//ゲーム情報
	int mposX, mposY;//マウス座標
	int posX, posY;
	bool in_out;  //マウスと接触したか調べる
	DG::Image::SP imgBG,imgBotan,imgTitle;
	XI::Mouse::SP mouse;
	ML::Box2D hitBase;

	//------------------------------------------------------
	//ゲーム初期化
	//-----------------------------------------------------
	void Initialize()
	{
		ge->dgi->EffectState().param.bgColor = ML::Color(0, 0, 0, 0);
		mouse = XI::Mouse::Create(2, 2);
		//画像の読み込み
		imgBG = DG::Image::Create("./data/image/GameBG.png");
		imgTitle = DG::Image::Create("./data/image/titleBG.png");
		imgBotan = DG::Image::Create("./data/image/botan.png");
		hitBase = ML::Box2D(180, 140, 125, 75);

		mposX = 0;
		mposY = 0;
		in_out = false;
	}
	//------------------------------------------------------
	//------------------------------------------------------
	void  Finalize()
	{
		imgBotan.reset();
	}
	//-------------------------------------------------------
	//更新処理
	//-------------------------------------------------------
    TaskFlag  UpDate()
	{
		TaskFlag rtv = TaskFlag::Title;  //現在のタスクを指定
		auto inp = ge->in1->GetState();
		auto ms = mouse->GetState();
		
		//ボタンとマウスの接触判定
		mposX = ms.pos.x;
		mposY = ms.pos.y;
		in_out = false;
		if (hitBase.x <= mposX) {
			if (mposX < hitBase.x + hitBase.w) {
				if (hitBase.y <= mposY) {
					if(mposY < hitBase.y + hitBase.h) {
						in_out = true;
						if (ms.LB.down) {
							rtv = TaskFlag::Game;
						}
					}
				}
			}
		}
	
		return rtv;
	}
	//--------------------------------------------------------
	//描画処理
	//--------------------------------------------------------
	void  Render()
	{
		

		//背景	
		ML::Box2D draw0((480 - 34 * 8) / 2, (270 - 34 * 8) / 2, 480, 280);
		ML::Box2D src0(0, 0, 480, 280);
		imgBG->Draw(draw0, src0);

		//ボタンの描画
		ML::Box2D draw1 = hitBase;
		draw1.Offset(posX, posY);
		ML::Box2D src1(0, 0, 500, 300);
		imgBotan->Draw(draw1, src1);
		
		//タイトルロゴ
		ML::Box2D draw2(180, 50, 125, 75);
		ML::Box2D src2(0, 0, 480, 270);
		imgTitle->Draw(draw2, src2);


	}

}