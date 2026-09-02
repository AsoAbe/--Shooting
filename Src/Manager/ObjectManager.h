#pragma once
#include <DxLib.h>
#include <vector>
#include "../Common/CharacterModelData.h"
#include "../Common/ObjectModelData.h"
#include "../Object/CharacterBase.h"
#include "../Object/ObjectBase.h"
#include "../Object/Shot/ShotBase.h"


class SceneGame;
class Player;
class Map;

class ObjectManager
{
public:
	ObjectManager(void);

	enum class ShotType
	{
		SHOT,			//標準
		SHOT_PLAYER,	//プレイヤーの弾
		SHOT_LOW,		//低威力低速度
		TEST			//テスト用
	};

	// 無効なインデックス
	static constexpr int INVALID_INDEX = -1;

	// 影のデフォルト半径
	static constexpr float DEFAULT_SHADOW_RADIUS = 60.0f;

	// 影を描画する際の下方向の距離
	static constexpr float SHADOW_LINE_LENGTH_DOWN = 1000.0f;

	// 影を描画する際の上方向の距離
	static constexpr float SHADOW_LINE_LENGTH_UP = 40.0f;

	// 影のアルファ値
	static constexpr int SHADOW_ALPHA = 114;

	// 影の頂点で使用するポリゴンの頂点数
	static constexpr int SHADOW_POLYGON_VERTEX_COUNT = 3;

	// 影を地面から浮かせる高さ
	static constexpr float SHADOW_Y_OFFSET = 4.0f;

	// 影の地面判定を分割する回数
	static constexpr int SHADOW_DIVISION_COUNT = 16;

	// 1回あたりの影の角度
	static constexpr float SHADOW_DIVISION_ANGLE =
		360.0f / SHADOW_DIVISION_COUNT;

	// 地面に当たらなかった場合の初期Y座標
	static constexpr float NO_GROUND_Y = -1.0f;

	// 影のテクスチャ座標の中心
	static constexpr float SHADOW_UV_CENTER = 0.5f;

	// 影のテクスチャ座標の半分
	static constexpr float SHADOW_UV_HALF = 0.5f;

	// 影の法線方向
	static constexpr float SHADOW_NORMAL_Z = -1.0f;

	// 影のスペキュラ色のアルファ値
	static constexpr int SHADOW_SPECULAR_ALPHA = 0;

	// 影のテクスチャ座標
	static constexpr float SHADOW_UV_ZERO = 0.0f;

	// ダメージ判定時の接触ダメージ
	static constexpr int DAMAGE_HIT = 2;

	// エフェクトの大きさ（敵弾・プレイヤー）
	static constexpr float HIT_EFFECT_SCALE = 0.8f;

	// エフェクトの大きさ（プレイヤー弾）
	static constexpr float ENEMY_HIT_EFFECT_SCALE = 0.4f;

	// 弾などの最大生成数
	static constexpr int INSTANCE_MAX = 999;

	//初期化
	bool Init(SceneGame* sceneGame);

	//毎フレーム実行処理
	void Update(void);
	//描画処理
	void Draw(void);

	//メモリ解放してインスタンスを破棄
	void Release(void);

	//プレイヤーのポインタ
	Player* GetPlayer(void);

	//マップのポインタ
	Map* GetMap(void);

	/// <summary>
	/// キャラクター追加(Init処理はここではしない)
	/// </summary>
	/// <param name="c">対象</param>
	/// <param name="pos">座標指定</param>
	void AddCharacter(CharacterBase* c,VECTOR pos);

	/// <summary>
	/// キャラクターをリリースしつつ配列から消去
	/// </summary>
	/// <param name="c">対象</param>
	void DestroyCharacter(CharacterBase* c);

	/// <summary>
	/// オブジェクト追加(Init処理はここではしない)
	/// </summary>
	/// <param name="o">対象</param>
	/// <param name="pos">座標指定</param>
	void AddObject(ObjectBase* o, VECTOR pos);

	/// <summary>
	/// オブジェクトをリリースしつつ配列から消去
	/// </summary>
	/// <param name="o">対象</param>
	void DestroyObject(ObjectBase* o);


	/// <summary>
	/// Shotオブジェクト追加(Init処理はここではしない)
	/// </summary>
	/// <param name="o">対象</param>
	void AddShotObj(ShotBase* o, VECTOR pos, Quaternion quaRot, float size);

	/// <summary>
	/// タイプ無関係に非アクティブな場所を探してoとして置き換えた後アクティブ化する
	/// </summary>
	/// <param name="o">対象</param>
	/// /// <returns>失敗した場合はfalse</returns>
	bool ReplaceShotObj(ShotBase* o, VECTOR pos, Quaternion quaRot, float size);

	/// <summary>
	/// 該当タイプの非アクティブなオブジェクトのインデックスを探す
	/// </summary>
	/// <param name="type">タイプ判別用</param>
	/// <returns>見つからない場合は-1</returns>
	int FindNotActiveShot(const type_info& type);
	/// <summary>
	/// 非アクティブなオブジェクトのインデックスを探す(タイプ関係なし)
	/// </summary>
	/// <returns></returns>
	int FindNotActiveShot();

	/// <summary>
	/// 新しい弾(再アクティブ化と新規生成)
	/// </summary>
	/// <param name="type"></param>
	void CreateShot(ShotType type, VECTOR pos, Quaternion quaRot, float size);

	/// <summary>
	/// CreateShotの再アクティブ化処理
	/// </summary>
	void ActivateShot(int index, VECTOR pos, Quaternion quaRot, float size);

	/// <summary>
	/// すべての弾を非アクティブ化する。
	/// </summary>
	void DeactivateAllShot();

	/// <summary>
	/// 足元の影を描画する(現在はy固定)
	/// </summary>
	/// <param name="pos"></param>
	void DrawShadow(const VECTOR& pos,float radius = 60);

	/// <summary>
	/// 座標からマップに線を飛ばしてヒットした座標(接地する座標)を返す
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="lengthUp">始点までの距離</param>
	/// <param name="lengthDown">終点までの距離</param>
	/// <returns>接地する座標Y、ヒットしなかった場合は終点の座標Y</returns>
	float GetOnGroundY(VECTOR pos, float lengthUp, float lengthDown)const;

	CharacterBase* GetTargetCharacter();
private:
	SceneGame* sceneGame_;
	Map* map_;
	Player* player_;
	
	std::vector<CharacterBase*> characters_;
	std::vector<ObjectBase*> objects_;
	std::vector<ShotBase*> shotObj_;

	CharacterModelData playerModel_;
	CharacterModelData testModel_;
	CharacterModelData golemModel_;
	ObjectModelData shotModel_;
	ObjectModelData rockModel_;

	//キャラの足元の影
	int shadowGraph_;

	//キャラクターのロード
	void LoadPlayerModel();

	void LoadTestModel();

	void LoadGolemModel();

	//Init内で呼ばれる
	void CreatePlayer();

	//キャラと弾の衝突判定。Update内で呼ばれる。
	void Collision();

	/// <summary>
	/// 球と球の判定
	/// </summary>
	/// <param name="oPos">対象1の座標</param>
	/// <param name="oRadius">対象1の半径</param>
	/// <param name="tPos">対象2の座標</param>
	/// <param name="tRadius">対象2の半径</param>
	/// <returns></returns>
	bool Collision_SphereAndSphere(const VECTOR& oPos, float oRadius, const VECTOR& tPos, float tRadius)const;
	/// <summary>
	/// 球と球の判定、ただし掠り判定はyの判定にoCRadiusを使用する
	/// </summary>
	/// <param name="oPos">対象1の座標</param>
	/// <param name="oGRadius">対象1の掠り判定半径</param>
	/// <param name="oCRadius">対象1の半径</param>
	/// <param name="tPos">対象2の座標</param>
	/// <param name="tRadius">対象2の半径</param>
	/// <returns></returns>
	bool Collision_GrazeSphereAndSphere(const VECTOR& oPos, float oGRadius, float oCRadius, const VECTOR& tPos, float tRadius)const;

	//当たり判定を個別に判定。プレイヤーと敵の弾
	bool Collision_PlayerAndEShot(const CharacterBase& player, const ShotBase& eShot)const;
	//当たり判定を個別に判定。敵とプレイヤーの弾
	bool Collision_EnemyAndPShot(const CharacterBase& enemy, const ShotBase& pShot)const;
	//当たり判定を個別に判定。キャラとキャラ
	bool Collision_CharacterAndCharacter(const CharacterBase& o, const CharacterBase& t)const;

	bool Collision_GrazeAndCharacter(const Player& p, const CharacterBase& e)const;
	bool Collision_GrazeAndEShot(const Player& p, const ShotBase& eShot)const;
};