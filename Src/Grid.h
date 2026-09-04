#pragma once
#include <DxLib.h>
class Grid
{
public:
	//変数
	static constexpr float LEN = 1200.0f;//長さ
	static constexpr float HLEN = LEN/2.0f;//長さの半分
	static constexpr float TERM = 50.0f;//間隔
	static constexpr float NUM = static_cast<int>(LEN/TERM);//線の数
	static constexpr float HNUM = NUM/2.0f;//線の数の半分

	// グリッド描画
	static constexpr int GRID_LINE_START = -1;
	static constexpr int GRID_LINE_END = 1;
	static constexpr int GRID_SPHERE_RADIUS = 8;
	static constexpr int GRID_SPHERE_SEGMENTS = 8;

	// グリッドの基準座標
	static constexpr float GRID_ORIGIN = 0.0f;

	// 描画色
	static constexpr int GRID_X_AXIS_COLOR = 0xFF0000;
	static constexpr int GRID_Y_AXIS_COLOR = 0x00FF00;
	static constexpr int GRID_Z_AXIS_COLOR = 0x0000FF;
	static constexpr int GRID_SPHERE_COLOR = 0x000000;

	//関数

	// コンストラクタ
	Grid(void);
	// デストラクタ
	~Grid(void);
	// 初期処理(基本的に最初の１回だけ実装)
	void Init(void);
	// 更新処理(毎フレーム実行)
	void Update(void);
	// 描画処理(毎フレーム実行)
	void Draw(void);
	// 解放処理(基本的に最後の１回だけ実装)
	void Release(void);

};
