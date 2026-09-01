#include <math.h>
#include <DxLib.h>
#include "AsoUtility.h"
#include "Quaternion.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
Quaternion::Quaternion(void)
{
    w = 1;
    x = y = z = 0;
}

Quaternion::Quaternion(const VECTOR& rad)
{
    Quaternion q = Euler(rad.x, rad.y, rad.z);
    w = q.w;
    x = q.x;
    y = q.y;
    z = q.z;
}

Quaternion::Quaternion(double ww, double wx, double wy, double wz)
{
    w = ww;
    x = wx;
    y = wy;
    z = wz;
}

Quaternion::~Quaternion(void)
{
}

Quaternion Quaternion::Euler(const VECTOR& rad)
{
    return Euler(rad.x, rad.y, rad.z);
}

Quaternion Quaternion::Euler(double radX, double radY, double radZ)
{

    Quaternion ret = Quaternion();

    radX = AsoUtility::RadIn2PI(radX);
    radY = AsoUtility::RadIn2PI(radY);
    radZ = AsoUtility::RadIn2PI(radZ);

    double cosZ = cos(radZ * HALF);
    double sinZ = sin(radZ * HALF);
    double cosX = cos(radX * HALF);
    double sinX = sin(radX * HALF);
    double cosY = cos(radY * HALF);
    double sinY = sin(radY * HALF);

    ret.w = cosX * cosY * cosZ + sinX * sinY * sinZ;
    ret.x = sinX * cosY * cosZ + cosX * sinY * sinZ;
    ret.y = cosX * sinY * cosZ - sinX * cosY * sinZ;
    ret.z = cosX * cosY * sinZ - sinX * sinY * cosZ;

    return ret;

}

Quaternion Quaternion::Mult(const Quaternion& q1, const Quaternion& q2)
{

    Quaternion ret = Quaternion();
    double d1, d2, d3, d4;

    // wÇÃåvéZ 
    d1 = q1.w * q2.w;
    d2 = -q1.x * q2.x;
    d3 = -q1.y * q2.y;
    d4 = -q1.z * q2.z;
    ret.w = d1 + d2 + d3 + d4;

    // xÇÃåvéZ 
    d1 = q1.w * q2.x;
    d2 = q2.w * q1.x;
    d3 = q1.y * q2.z;
    d4 = -q1.z * q2.y;
    ret.x = d1 + d2 + d3 + d4;

    // yÇÃåvéZ
    d1 = q1.w * q2.y;
    d2 = q2.w * q1.y;
    d3 = q1.z * q2.x;
    d4 = -q1.x * q2.z;
    ret.y = d1 + d2 + d3 + d4;

    // zÇÃåvéZ
    d1 = q1.w * q2.z;
    d2 = q2.w * q1.z;
    d3 = q1.x * q2.y;
    d4 = -q1.y * q2.x;
    ret.z = d1 + d2 + d3 + d4;

    return ret;

}

Quaternion Quaternion::Mult(const Quaternion& q) const
{
    return Mult(Quaternion(w, x, y, z), q);
}

Quaternion Quaternion::AngleAxis(double rad, VECTOR axis)
{

    Quaternion ret = Quaternion();

    double norm;
    double c, s;

    // UnityÇ…çáÇÌÇπÇÈ
    ret.w = IDENTITY_W;
    ret.x = ret.y = ret.z = IDENTITY_COMPONENT;


    norm = (double)axis.x * (double)axis.x + (double)axis.y * (double)axis.y + (double)axis.z * (double)axis.z;
    if (norm <= IDENTITY_COMPONENT)
    {
        return ret;
    }

    norm = IDENTITY_W / sqrt(norm);
    axis.x = (float)(axis.x * norm);
    axis.y = (float)(axis.y * norm);
    axis.z = (float)(axis.z * norm);

    c = cos(HALF * rad);
    s = sin(HALF * rad);

    ret.w = c;
    ret.x = s * axis.x;
    ret.y = s * axis.y;
    ret.z = s * axis.z;

    return ret;

}

VECTOR Quaternion::PosAxis(const Quaternion& q, VECTOR pos)
{
    // à íuèÓïÒÇ…âÒì]èÓïÒÇîΩâfÇ≥ÇπÇÈ
    Quaternion tmp = Quaternion();
    tmp = tmp.Mult(q);
    tmp = tmp.Mult(Quaternion(IDENTITY_COMPONENT, pos.x, pos.y, pos.z));
    tmp = tmp.Mult(q.Inverse());
    return { (float)tmp.x, (float)tmp.y, (float)tmp.z };
}

VECTOR Quaternion::PosAxis(VECTOR pos) const
{
    return PosAxis(Quaternion(w, x, y, z), pos);
}

VECTOR Quaternion::ToEuler(const Quaternion& q)
{

    VECTOR ret;

    double r11 = 2 * (q.x * q.z + q.w * q.y);
    double r12 = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;
    double r21 = -2 * (q.y * q.z - q.w * q.x);
    double r31 = 2 * (q.x * q.y + q.w * q.z);
    double r32 = q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z;

    ret.x = static_cast<float>(asin(r21));
    ret.y = static_cast<float>(atan2(r11, r12));
    ret.z = static_cast<float>(atan2(r31, r32));

    return ret;

}

VECTOR Quaternion::ToEuler(void) const
{
    return ToEuler(Quaternion(w, x, y, z));
}

MATRIX Quaternion::ToMatrix(const Quaternion& q)
{

    MATRIX mat;

    FLOAT4 fq = { (float)q.x, (float)q.y, (float)q.z, (float)q.w };

    float sx = fq.x * fq.x * 2.0f;
    float sy = fq.y * fq.y * 2.0f;
    float sz = fq.z * fq.z * 2.0f;
    float cx = fq.y * fq.z * 2.0f;
    float cy = fq.x * fq.z * 2.0f;
    float cz = fq.x * fq.y * 2.0f;
    float wx = fq.w * fq.x * 2.0f;
    float wy = fq.w * fq.y * 2.0f;
    float wz = fq.w * fq.z * 2.0f;

    mat.m[0][0] = 1.0f - (sy + sz);	mat.m[0][1] = cz + wz;			mat.m[0][2] = cy - wy;			mat.m[0][3] = 0.0f;
    mat.m[1][0] = cz - wz;			mat.m[1][1] = 1.0f - (sx + sz);	mat.m[1][2] = cx + wx;			mat.m[1][3] = 0.0f;
    mat.m[2][0] = cy + wy;			mat.m[2][1] = cx - wx;			mat.m[2][2] = 1.0f - (sx + sy);	mat.m[2][3] = 0.0f;
    mat.m[3][0] = 0.0f;				mat.m[3][1] = 0.0f;				mat.m[3][2] = 0.0f;				mat.m[3][3] = 1.0f;

    return mat;

}

MATRIX Quaternion::ToMatrix(void) const
{
    return ToMatrix(Quaternion(w, x, y, z));
}

Quaternion Quaternion::LookRotation(VECTOR dir)
{
    VECTOR up = AsoUtility::DIR_U;
    return LookRotation(dir, up);
}

Quaternion Quaternion::LookRotation(VECTOR dir, VECTOR up)
{

    dir = AsoUtility::VNormalize(dir);
    VECTOR right = AsoUtility::VNormalize(VCross(up, dir));
    up = VCross(dir, right);
    auto m00 = right.x;
    auto m01 = right.y;
    auto m02 = right.z;
    auto m10 = up.x;
    auto m11 = up.y;
    auto m12 = up.z;
    auto m20 = dir.x;
    auto m21 = dir.y;
    auto m22 = dir.z;


    float num8 = (m00 + m11) + m22;
    auto quaternion = Quaternion();
    if (num8 > 0.0f)
    {
        double num = sqrt(num8 + 1.0);
        quaternion.w = num * 0.5;
        num = 0.5 / num;
        quaternion.x = ((double)m12 - m21) * num;
        quaternion.y = ((double)m20 - m02) * num;
        quaternion.z = ((double)m01 - m10) * num;
        return quaternion.Normalized();
    }
    if ((m00 >= m11) && (m00 >= m22))
    {
       
        auto num7 = sqrt(((1.0f + m00) - m11) - m22);
        auto num4 = 0.5f / num7;
        quaternion.x = ((double)m12 - m21) * num4;
        quaternion.y = ((double)m01 + m10) * num4;
        quaternion.z = ((double)m02 + m20) * num4;
        quaternion.w = 0.5 * num7;
        return quaternion.Normalized();
    }
    if (m11 > m22)
    {
        auto num6 = sqrt(((1.0f + m11) - m00) - m22);
        auto num3 = 0.5f / num6;
        quaternion.x = ((double)m10 + m01) * num3;
        quaternion.y = 0.5 * num6;
        quaternion.z = ((double)m21 + m12) * num3;
        quaternion.w = ((double)m20 - m02) * num3;
        return quaternion.Normalized();
    }
    auto num5 = sqrt(((1.0f + m22) - m00) - m11);
    auto num2 = 0.5f / num5;
    quaternion.x = ((double)m20 + m02) * num2;
    quaternion.y = ((double)m21 + m12) * num2;
    quaternion.z = 0.5 * num5;
    quaternion.w = ((double)m01 - m10) * num2;
    return quaternion.Normalized();

}

Quaternion Quaternion::GetRotation(MATRIX mat)
{

    Quaternion ret;

    float s;
    float tr = mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + 1.0f;
    if (tr >= 1.0f)
    {
        s = 0.5f / sqrtf(tr);
        ret.w = 0.25f / s;
        ret.x = (mat.m[1][2] - mat.m[2][1]) * s;
        ret.y = (mat.m[2][0] - mat.m[0][2]) * s;
        ret.z = (mat.m[0][1] - mat.m[1][0]) * s;
    }
    else
    {
        float max;
        max = mat.m[1][1] > mat.m[2][2] ? mat.m[1][1] : mat.m[2][2];

        if (max < mat.m[0][0])
        {
            s = sqrtf(mat.m[0][0] - (mat.m[1][1] + mat.m[2][2]) + 1.0f);

            float x = s * 0.5f;
            s = 0.5f / s;
            ret.x = x;
            ret.y = (mat.m[0][1] + mat.m[1][0]) * s;
            ret.z = (mat.m[2][0] + mat.m[0][2]) * s;
            ret.w = (mat.m[1][2] - mat.m[2][1]) * s;
        }
        else
            if (max == mat.m[1][1])
            {
                s = sqrtf(mat.m[1][1] - (mat.m[2][2] + mat.m[0][0]) + 1.0f);

                float y = s * 0.5f;
                s = 0.5f / s;
                ret.x = (mat.m[0][1] + mat.m[1][0]) * s;
                ret.y = y;
                ret.z = (mat.m[1][2] + mat.m[2][1]) * s;
                ret.w = (mat.m[2][0] - mat.m[0][2]) * s;
            }
            else
            {
                s = sqrtf(mat.m[2][2] - (mat.m[0][0] + mat.m[1][1]) + 1.0f);

                float z = s * 0.5f;
                s = 0.5f / s;
                ret.x = (mat.m[2][0] + mat.m[0][2]) * s;
                ret.y = (mat.m[1][2] + mat.m[2][1]) * s;
                ret.z = z;
                ret.w = (mat.m[0][1] - mat.m[1][0]) * s;
            }
    }

    return ret;


}

VECTOR Quaternion::GetDir(VECTOR dir) const
{
    return PosAxis(dir);
}

VECTOR Quaternion::GetForward(void) const
{
    return GetDir(AsoUtility::DIR_F);
}

VECTOR Quaternion::GetBack(void) const
{
    return GetDir(AsoUtility::DIR_B);
}

VECTOR Quaternion::GetRight(void) const
{
    return GetDir(AsoUtility::DIR_R);
}

VECTOR Quaternion::GetLeft(void) const
{
    return GetDir(AsoUtility::DIR_L);
}

VECTOR Quaternion::GetUp(void) const
{
    return GetDir(AsoUtility::DIR_U);
}

VECTOR Quaternion::GetDown(void) const
{
    return GetDir(AsoUtility::DIR_D);
}

double Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
{
    return (q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z);
}

double Quaternion::Dot(const Quaternion& q) const
{
    return (w * q.w + x * q.x + y * q.y + z * q.z);
}

Quaternion Quaternion::Normalize(const Quaternion& q)
{
    float scale = 1.0f / static_cast<float>(q.Length());
    VECTOR v = VScale(q.xyz(), scale);
    Quaternion ret = Quaternion(q.w * scale, v.x, v.y, v.z);
    return ret;
}

Quaternion Quaternion::Normalized(void) const
{

    double mag = sqrt(w * w + x * x + y * y + z * z);
    return Quaternion(w / mag, x / mag, y / mag, z / mag);

}

void Quaternion::Normalize(void)
{

    double mag = sqrt(w * w + x * x + y * y + z * z);

    w /= mag;
    x /= mag;
    y /= mag;
    z /= mag;

}

Quaternion Quaternion::Inverse(void) const
{

    double n = IDENTITY_W / (w * w + x * x + y * y + z * z);
    Quaternion tmp = Quaternion(w, -x, -y, -z);
    return Quaternion(tmp.w * n, tmp.x * n, tmp.y * n, tmp.z * n);;

}

Quaternion Quaternion::Slerp(Quaternion from, Quaternion to, double t)
{
    if (t > UNIT_LENGTH)
    {
        t = UNIT_LENGTH;
    }

    if (t < IDENTITY_COMPONENT)
    {
        t = IDENTITY_COMPONENT;
    }

    return SlerpUnclamped(from, to, (float)t);

}

inline float SIGN(float x) {
    return (x >= 0.0f) ? +1.0f : -1.0f;
}

inline float NORM(float a, float b, float c, float d) {
    return sqrt(a * a + b * b + c * c + d * d);
}

Quaternion Quaternion::FromToRotation(VECTOR fromDir, VECTOR toDir)
{

	VECTOR axis = VCross(fromDir, toDir);
	double angle = AsoUtility::AngleDeg(fromDir, toDir);
	if (angle >= NEAR_180_DEGREES)
	{
		auto r = VCross(fromDir, AsoUtility::DIR_R);
		axis = VCross(r, fromDir);
		float len = axis.x * axis.x + axis.y * axis.y + axis.z * axis.z;
		if (len < EPSILON_LENGTH_SQUARED)
		{
			axis = AsoUtility::DIR_U;
		}
	}

	axis = AsoUtility::VNormalize(axis);
	return Quaternion::AngleAxis(AsoUtility::Deg2RadD(angle), axis);

}

Quaternion Quaternion::RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta)
{
    double num = Quaternion::Angle(from, to);
    if (num == IDENTITY_COMPONENT)
    {
        return to;
    }
    float t = min(static_cast<float>(UNIT_LENGTH), maxDegreesDelta / (float)num);
    return Quaternion::SlerpUnclamped(from, to, t);
}

double Quaternion::Angle(const Quaternion& q1, const Quaternion& q2)
{
	double cos = Quaternion::Dot(q1, q2);
    double ac = acos(cos);
	return ac * (180.0 / DX_PI);
}

Quaternion Quaternion::SlerpUnclamped(Quaternion a, Quaternion b, float t)
{

    if (a.LengthSquared() == IDENTITY_COMPONENT)
    {
        if (b.LengthSquared() == IDENTITY_COMPONENT)
        {
            return Identity();
        }
        return b;
    }
    else if (b.LengthSquared() == IDENTITY_COMPONENT)
    {
        return a;
    }


    float cosHalfAngle = (float)(a.w * b.w) + VDot(a.xyz(), b.xyz());

    if (cosHalfAngle >= UNIT_LENGTH || cosHalfAngle <= REVERSE)
    {
        return a;
    }
    else if (cosHalfAngle < IDENTITY_COMPONENT)
    {
		b.x = b.x * REVERSE;
		b.y = b.y * REVERSE;
		b.z = b.z * REVERSE;
		b.w = b.w * REVERSE;
        cosHalfAngle = -cosHalfAngle;
    }

    float blendA;
    float blendB;
    if (cosHalfAngle < SLERP_THRESHOLD)
    {
        float halfAngle = acosf(cosHalfAngle);
        float sinHalfAngle = sinf(halfAngle);
        float oneOverSinHalfAngle = 1.0f / sinHalfAngle;
        blendA = sinf(halfAngle * (1.0f - t)) * oneOverSinHalfAngle;
        blendB = sinf(halfAngle * t) * oneOverSinHalfAngle;
    }
    else
    {
        blendA = 1.0f - t;
        blendB = t;
    }

    VECTOR v = VAdd(VScale(a.xyz(), blendA), VScale(b.xyz(), blendB));
	Quaternion result = Quaternion(blendA * a.w + blendB * b.w, v.x, v.y, v.z);
    if (result.LengthSquared() > IDENTITY_COMPONENT)
    {
        return Normalize(result);
    }
    else
    {
        return Identity();
    }

}

Quaternion Quaternion::Identity(void)
{
    return Quaternion(
		IDENTITY_W,
		IDENTITY_COMPONENT,
		IDENTITY_COMPONENT,
		IDENTITY_COMPONENT);
}

double Quaternion::Length(void) const
{
    return sqrt(x * x + y * y + z * z + w * w);
}

double Quaternion::LengthSquared(void) const
{
    return x * x + y * y + z * z + w * w;
}

VECTOR Quaternion::xyz(void) const
{
    return { (float)x, (float)y, (float)z };
}

void Quaternion::ToAngleAxis(float* angle, VECTOR* axis)
{

	if (abs(this->w) > MAX_ROTATION_VALUE)
	{
		this->Normalize();
	}
	*angle = 2.0f * acosf((float)this->w); // angle

    if (x == IDENTITY_COMPONENT &&
        y == IDENTITY_COMPONENT &&
        z == IDENTITY_COMPONENT)
    {
        *angle = IDENTITY_COMPONENT;
    }

	float den = sqrtf(MAX_ROTATION_VALUE - (float)(this->w * this->w));
	if (den > EPSILON_AXIS)
	{
		auto v = this->xyz();
		axis->x = v.x / den;
		axis->y = v.y / den;
		axis->z = v.z / den;
	}
	else
	{
        *axis = {
               static_cast<float>(IDENTITY_W),
               static_cast<float>(IDENTITY_COMPONENT),
               static_cast<float>(IDENTITY_COMPONENT)
        };
	}

}

Quaternion Quaternion::operator*(float& f) {
    return Quaternion(w * f, x * f, y * f, z * f);
}

const Quaternion Quaternion::operator*(const float& f) {
    return Quaternion(w * f, x * f, y * f, z * f);
}

Quaternion Quaternion::operator+(Quaternion& rhs) {
    return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
}

const Quaternion Quaternion::operator+(const Quaternion& rhs) {
    return Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
}
