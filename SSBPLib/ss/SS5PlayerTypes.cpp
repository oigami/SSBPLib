#include "SS5PlayerTypes.h"
#include "player/DataArrayReader.h"
#include "player/PlayerDef.h"

namespace ss{

//カラーの読み取り
void SSColor4B::readColor(DataArrayReader& reader){
	unsigned int raw = reader.readU32();
	a = static_cast<unsigned char>(raw >> 24);
	r = static_cast<unsigned char>(raw >> 16);
	g = static_cast<unsigned char>(raw >> 8);
	b = static_cast<unsigned char>(raw);
}

//rateを考慮して読む
void SSColor4B::readColorWithRate(DataArrayReader& reader){
	float blend_rate = reader.readFloat();					//レート読み込み
	readColor(reader);

	/*
	 * 本家のssbpLibの2015/10/09での修正で
	 *  //ssbpではカラーブレンドのレート（％）は使用できません。
	 *  //制限となります。
	 * とコメントがあるが謎いのでアルファへの適用を復活させておきます
	 * カラーの情報がおかしいときはここのアルファへの処理をコメントアウトして下さい
	 */
	a = static_cast<unsigned int>(blend_rate * 255);	//レートをアルファ値として設定
}


//unsigned long化
static unsigned long toLong(unsigned int x, unsigned int y, unsigned int z, unsigned int w){
	return ((unsigned long)((((x) & 0xff) << 24) | (((y) & 0xff) << 16) | (((z) & 0xff) << 8) | ((w) & 0xff)));
}
unsigned long SSColor4B::packARGB() const{
	return toLong(a, r, g, b);
}
unsigned long SSColor4B::packRGBA() const{
	return toLong(r, g, b, a);
}
unsigned long SSColor4B::packABGR() const{
	return toLong(a, b, g, r);
}
unsigned long SSColor4B::packBGRA() const{
	return toLong(b, g, r, a);
}

//型変換
SSColorF SSColor4B::toSSColorF() const{
	return SSColorF(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}


//float版--------------------------------------------------
//カラーの読み取り
void SSColorF::readColor(DataArrayReader& reader){
	SSColor4B color;
	color.readColor(reader);
	*this = color.toSSColorF();
}

//rateを考慮して読む
void SSColorF::readColorWithRate(DataArrayReader& reader){
	SSColor4B color;
	color.readColorWithRate(reader);
	*this = color.toSSColorF();
}


//unsigned long化
unsigned long SSColorF::packARGB() const{
	return toSSColor4B().packARGB();
}
unsigned long SSColorF::packRGBA() const{
	return toSSColor4B().packRGBA();
}
unsigned long SSColorF::packABGR() const{
	return toSSColor4B().packABGR();
}
unsigned long SSColorF::packBGRA() const{
	return toSSColor4B().packBGRA();
}

//型変換
SSColor4B SSColorF::toSSColor4B() const{
	return SSColor4B(
		static_cast<unsigned char>(r*255.0f),
		static_cast<unsigned char>(g*255.0f),
		static_cast<unsigned char>(b*255.0f),
		static_cast<unsigned char>(a*255.0f)
	);
}




//頂点オフセットの読み取り
void SSQuad3::readData(DataArrayReader &reader)
{
	tl = Vector3::zero;
	tr = Vector3::zero;
	bl = Vector3::zero;
	br = Vector3::zero;

	int vt_flags = reader.readU16();
	if (vt_flags & VERTEX_FLAG_LT){
		tl.x = reader.readS16();
		tl.y = reader.readS16();
	}
	if (vt_flags & VERTEX_FLAG_RT){
		tr.x = reader.readS16();
		tr.y = reader.readS16();
	}
	if (vt_flags & VERTEX_FLAG_LB){
		bl.x = reader.readS16();
		bl.y = reader.readS16();
	}
	if (vt_flags & VERTEX_FLAG_RB){
		br.x = reader.readS16();
		br.y = reader.readS16();
	}
}


//カラーブレンドの読み取り
void SSQuadColor::readData(int vertexFlags, DataArrayReader& reader)
{
	if(vertexFlags & VERTEX_FLAG_ONE){
		SSColor4B color;
		color.readColorWithRate(reader);

		tl = tr = bl = br = color;
	}
	else{
		tl = tr = bl = br = SSColor4B(0xff, 0xff, 0xff, 0xff);

		if(vertexFlags & VERTEX_FLAG_LT){
			tl.readColorWithRate(reader);
		}
		if(vertexFlags & VERTEX_FLAG_RT){
			tr.readColorWithRate(reader);
		}
		if(vertexFlags & VERTEX_FLAG_LB){
			bl.readColorWithRate(reader);
		}
		if(vertexFlags & VERTEX_FLAG_RB){
			br.readColorWithRate(reader);
		}
	}
}


} //namespace ss
