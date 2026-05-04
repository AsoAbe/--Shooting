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
