#pragma once

class Player;

struct ScoreData;

class ScoreManager
{
public:

	//インスタンスの生成(シングルトン)(外部から静的にインスタンスを生成)
	static void CreateInstance(void);
	//インスタンスを返す
	static ScoreManager& GetInstance();

	//メモリ解放してインスタンスを破棄
	void Release(void);

	int GetScore()const;
	int GetHighScore()const;

	/// <summary>
	/// スコアをセットする。ハイスコアを更新した場合はtrueを返す
	/// </summary>
	/// <returns>ハイスコアを更新した場合true</returns>
	bool SetScore(int score);

	/// <summary>
	/// スコアとハイスコアを初期値に戻す
	/// </summary>
	void ClearScore();

	/// <summary>
	/// スコア計算
	/// </summary>
	/// <param name="targetProgression">1以上で完全。1を下回る場合はスコア減少</param>
	/// <param name="player">プレイヤーキャラ</param>
	/// <param name="timerS">経過時間。秒数で入力すること</param>
	/// <returns>計算結果</returns>
	static ScoreData CalcScore(float targetProgression,const Player* player,float timerS);
private:
	//インスタンス
	static ScoreManager* instance_;

	int score_;
	int highScore_;

	//コンストラクタ
	ScoreManager(void);
	//コピーのコンストラクタも潰す(privateに)
	ScoreManager(const ScoreManager& ins) = delete;

	//デストラクタ
	~ScoreManager(void);
};

