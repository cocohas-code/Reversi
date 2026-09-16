#include "MyPG.h"
#include "MyGameMain.h"

namespace Game
{
	//ゲーム情報
	//コマが置いてるか、置いてないか
	enum class Stone { White, Brack, Non };

	struct MapData {
		int arr[8][8];
		ML::Box2D chip[3];
	};

	
	Stone turn;
	MapData mapdata;
	DG::Image::SP imgBG, imgKoma;
	DG::Font::SP font;
	XI::Mouse::SP mouse;
	ML::Point mPos, mapDrawOffset;

	//プロトタイプ宣言
	bool Map_ChangeChip(MapData& md_, ML::Point p_, Stone stone_);//コマの範囲チェック
	void Koma_returntrue(MapData& md_, ML::Point p_, Stone turn_);
	void Koma_Checkreturn(MapData& md_, ML::Point p, Stone turn_, int y, int x);
	void Map_Render(); //ゲーム盤の描画
	void font_Render(Stone turn_);



	//-----------------------------------------------------
	//ゲーム初期化
	//-----------------------------------------------------
	void Initialize()
	{
		imgBG = DG::Image::Create("./data/image/GameBG.png");	//背景
		imgKoma = DG::Image::Create("./data/image/koma.png");	//コマ
		mouse = XI::Mouse::Create(2, 2);						//マウス座標
		turn = Stone::Brack;									//順番（最初は黒のコマから）
		font = DG::Font::Create("HGS 教科書体", 8, 16);
		mapDrawOffset.x = (480 - 34 * 8) / 2;
		mapDrawOffset.y = (270 - 34 * 8) / 2;

		//マップチップ初期化(コマ）
		for (int c = 0; c < 3; ++c)
		{
			int x = (c % 3);
			int y = (c / 3);
			mapdata.chip[c] = ML::Box2D(x * 34, y * 34, 34, 34);
		}

		//配列情報の初期化
		int w_map[8][8] = {
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,2,1,0,0,0},
			{0,0,0,1,2,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
		};

		for (int y = 0; y < 8; ++y) {
			for (int x = 0; x < 8; ++x)
			{
				mapdata.arr[y][x] = w_map[y][x];
			}
		}

	}
	//------------------------------------------------------
	//------------------------------------------------------
	void  Finalize()
	{
		imgBG.reset();
		imgKoma.reset();
		font.reset();
		mouse.reset();
	}
	//-------------------------------------------------------
	//更新処理
	//-------------------------------------------------------
	TaskFlag  UpDate()
	{
		auto inp = ge->in1->GetState();
		auto ms = mouse->GetState();
		mPos.x = ms.pos.x;
		mPos.y = ms.pos.y;


		if (ms.LB.down)
		{
			//クリックしたときの処理・判定
			ML::Point mp = ms.pos;
			ML::Rect sb = { mapDrawOffset.x, mapDrawOffset.y,
							mapDrawOffset.x + (34 * 8),
							mapDrawOffset.y + (34 * 8)
			};

			if (mp.x >= sb.left && mp.x < sb.right &&
				mp.y >= sb.top && mp.y < sb.bottom)
			{
				ML::Point mp2 = { mp.x - sb.left, mp.y - sb.top };
				ML::Point mp3 = { mp2.x / 34, mp2.y / 34 };
				//マス内にコマを置けるかチェック
				bool set = Map_ChangeChip(mapdata, mp3, turn);
				//順番の切り替え
				if (set == true) {
					if (turn == Stone::Brack) { turn = Stone::White; }
					else if (turn == Stone::White) { turn = Stone::Brack; }
				}

			}
		}

		TaskFlag rtv = TaskFlag::Game;//取りあえず現在のタスクを指示

		if (true == inp.SE.down) {
			rtv = TaskFlag::Title;
		}
		return rtv;
	}
	//--------------------------------------------------------
	//描画処理
	//--------------------------------------------------------
	void  Render()
	{
		//ゲーム盤
		Map_Render();
		//フォント
		font_Render(turn);

		//コマ描画
		for (int y = 0; y < 8; ++y) {
			for (int x = 0; x < 8; ++x)
			{
				ML::Box2D draw(x * 34, y * 34, 34, 34);
				draw.Offset(mapDrawOffset.x, mapDrawOffset.y);
				int num = mapdata.arr[y][x];
				ML::Box2D src = mapdata.chip[num];
				imgKoma->Draw(draw, src);

			}
		}

	}
	//--------------------------------------------------------
	//コマを置けるか範囲チェック
	//--------------------------------------------------------
	bool Map_ChangeChip(MapData& md_, ML::Point p_, Stone stone_)
	{
		//盤内
		if (p_.x < 0) { return false; }
		if (p_.y < 0) { return false; }
		if (p_.x >= 9) { return false; }
		if (p_.y >= 9) { return false; }

		//指定座標が埋まってる
		if (md_.arr[p_.y][p_.x] == 1)
		{
			return false;
		}
		if (md_.arr[p_.y][p_.x] == 2)
		{
			return false;
		}

		//クリックするとコマを置く
		if (stone_ == Stone::Brack)
		{
			Koma_returntrue(md_, p_, stone_);
			md_.arr[p_.y][p_.x] += 1;
			return true;
		}
		else if (stone_ == Stone::White)
		{
			Koma_returntrue(md_, p_, stone_);
			md_.arr[p_.y][p_.x] += 2;
			return true;
		}

	}
	//--------------------------------------------------------
	//ゲーム盤の描画
	//--------------------------------------------------------
	void Map_Render()
	{
		//ゲーム盤
		ML::Box2D draw(0, 0, 270, 270);
		draw.Offset(mapDrawOffset.x, mapDrawOffset.y);
		ML::Box2D src(0, 0, 270, 270);
		imgBG->Draw(draw, src);
	}
	//--------------------------------------------------------
	//コマ裏返しができるかチェック
	//--------------------------------------------------------
	void Koma_Checkreturn(MapData& md_, ML::Point p, Stone turn_, int y, int x)
	{
		for (int s = 1; s < 8; s++)
		{
			//盤面外なら終わり
			if (p.x + x * s < 0) { break; }//ループ終了
			if (p.y + y * s < 0) { break; }//ループ終了
			if (p.x + x * s >= 9) { break; }//ループ終了
			if (p.y + y * s >= 9) { break; }//ループ終了
			//空きなら終わり
			if (md_.arr[p.y + y * s][p.x + x * s] == 0)
			{
				break;
				//ループ終了
			}

			//もし、隣に自分の駒があれば
			//-----------------------------------------
			//黒の場合
			if (turn_ == Stone::Brack)
			{
				if (md_.arr[p.y + y * s][p.x + x * s] == 1)
				{
					for (int n = 1; n < s; n++)
					{
						md_.arr[p.y + y * n][p.x + x * n] = 1;
						
					}
					break;
				}

			}
			//白の場合
			if (turn_ == Stone::White)
			{
				if (md_.arr[p.y + y * s][p.x + x * s] == 2)
				{
					for (int n = 1;n < s; n++)
					{
						md_.arr[p.y + y * n][p.x + x * n] = 2;
					}
					break;
				}
			}
			//-----------------------------------------
		}
	}
	//--------------------------------------------------------
	//八方向のチェック（上下左右斜め）
	//--------------------------------------------------------
	void Koma_returntrue(MapData& md_, ML::Point p_, Stone turn_)
	{
		//八方向	
		ML::Point tbl[]  = {
			{0,-1},//左
			{0,1}, //右
			{-1,0},//上
			{1,0}, //下
			{-1,-1},//左上
			{1,1},  //右下
			{1,-1}, //左下
			{-1,1}	//右上
		};

		for (const auto& i : tbl) {
			Koma_Checkreturn(md_, p_, turn_, i.x,i.y);/*左*/
		}
	}
	void font_Render(Stone turn_)
	{
		if (turn_ == Stone::Brack)
		{
			ML::Box2D textBox(0, 0, 480, 48);
			string text = "黒のターンです";
			font->Draw(textBox, text, ML::Color(1.0f, 0.0f, 0.8f, 0.0f));
		}
		if (turn_ == Stone::White)
		{
			ML::Box2D textBox(0, 0, 480, 48);
			string text = "白のターンです";
			font->Draw(textBox, text, ML::Color(1.0f, 0.0f, 0.8f, 0.0f));
		}


	}

}

