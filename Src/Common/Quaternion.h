#pragma once
#include <DxLib.h>
#include <iostream>
#include <algorithm>
class Quaternion
{

public:

	// 正規化判定用の許容誤差
	static constexpr float EPSILON_NORMAL_SQRT = 1e-15F;

	// クォータニオンの単位長
	static constexpr double UNIT_LENGTH = 1.0;

	// クォータニオンの実部
	static constexpr double IDENTITY_W = 1.0;

	// クォータニオンの虚部
	static constexpr double IDENTITY_COMPONENT = 0.0;

	// 半分を表す値
	static constexpr double HALF = 0.5;

	// 逆方向
	static constexpr double REVERSE = -1.0;

	// ほぼ180度と判定する角度
	static constexpr double NEAR_180_DEGREES = 179.9196;

	// ほぼゼロと判定する長さの二乗
	static constexpr float EPSILON_LENGTH_SQUARED = 0.000001f;

	// Slerpで線形補間に切り替える閾値
	static constexpr float SLERP_THRESHOLD = 0.99f;

	// 回転値の上限
	static constexpr float MAX_ROTATION_VALUE = 1.0f;

	// 回転軸を判定する許容値
	static constexpr float EPSILON_AXIS = 0.0001f;
	double w;
	double x;
	double y;
	double z;

	Quaternion(void);
	Quaternion(const VECTOR& rad);
	Quaternion(double w, double x, double y, double z);

	~Quaternion(void);

	// オイラー角からクォータニオンへ変換
	static Quaternion Euler(const VECTOR& rad);
	static Quaternion Euler(double radX, double radY, double radZ);

	// クォータニオンの合成
	static Quaternion Mult(const Quaternion& q1, const Quaternion& q2);
	Quaternion Mult(const Quaternion& q) const;

	// 指定軸を指定角分、回転させる
	static Quaternion AngleAxis(double rad, VECTOR axis);

	// 座標を回転させる
	static VECTOR PosAxis(const Quaternion& q, VECTOR pos);
	VECTOR PosAxis(VECTOR pos) const;

	// クォータニオンからオイラー角へ変換
	static VECTOR ToEuler(const Quaternion& q);
	VECTOR ToEuler(void) const;

	// クォータニオンから行列へ変換
	static MATRIX ToMatrix(const Quaternion& q);
	MATRIX ToMatrix(void) const;

	// ベクトルからクォータニオンに変換
	static Quaternion LookRotation(VECTOR dir);
	static Quaternion LookRotation(VECTOR dir, VECTOR up);

	// 行列からクォータニオンに変換
	static Quaternion GetRotation(MATRIX mat);

	// 基本ベクトルを取得
	VECTOR GetForward(void) const;
	VECTOR GetBack(void) const;
	VECTOR GetRight(void) const;
	VECTOR GetLeft(void) const;
	VECTOR GetUp(void) const;
	VECTOR GetDown(void) const;

	// 内積
	static double Dot(const Quaternion& q1, const Quaternion& q2);
	double Dot(const Quaternion& b) const;

	// 正規化
	static Quaternion Normalize(const Quaternion& q);
	Quaternion Normalized(void) const;
	void Normalize(void);

	// 逆クォータニオン
	Quaternion Inverse(void) const;

	// 球面補間
	static Quaternion Slerp(Quaternion from, Quaternion to, double t);

	// ２つのベクトル間の回転量を取得する
	static Quaternion FromToRotation(VECTOR fromDir, VECTOR toDir);
	static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta);
	static double Angle(const Quaternion& q1, const Quaternion& q2);
	static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t);
	static Quaternion Identity(void);

	double Length(void) const;
	double LengthSquared(void) const;
	VECTOR xyz(void) const;

	// 対象方向の回転
	void ToAngleAxis(float* angle, VECTOR* axis);

private:

	// 基本ベクトルを取得
	VECTOR GetDir(VECTOR dir) const;

	Quaternion operator*(float& rhs);
	const Quaternion operator*(const float& rhs);
	Quaternion operator+(Quaternion& rhs);
	const Quaternion operator+(const Quaternion& rhs);
	
};
