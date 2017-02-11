#include "CustomSprite.h"
#include "Util.h"

namespace ss{

/**
 * CustomSprite
 */
 //カラーブレンド用のシェーダー処理は汎用的に使用する事ができないためすべてコメントにしてあります。
 //カラーブレンドを再現するための参考にしてください。

unsigned int CustomSprite::ssSelectorLocation = 0;
unsigned int CustomSprite::ssAlphaLocation = 0;
unsigned int CustomSprite::sshasPremultipliedAlpha = 0;

CustomSprite::CustomSprite() :
	_useCustomShaderProgram(false)
	, _opacity(1.0f)
	, _colorBlendFuncNo(0)
	, _liveFrame(0.0f)
	, _hasPremultipliedAlpha(0)
	, refEffect(0)
	, _ssplayer(0)
	, effectAttrInitialized(false)
	, effectTimeTotal(0)
{
	_temp_position = SsVector3(0, 0, 0);
	_temp_rotation = SsVector3(0, 0, 0);
	_temp_scale = SsVector2(1.0f, 1.0f);
}

CustomSprite::~CustomSprite()
{
	//エフェクトクラスがある場合は解放する
	SS_SAFE_DELETE(refEffect);
	SS_SAFE_DELETE(_ssplayer);
}


CustomSprite* CustomSprite::create()
{
	CustomSprite *pSprite = new CustomSprite();
	if(pSprite){
		pSprite->initState();
		return pSprite;
	}
	SS_SAFE_DELETE(pSprite);
	return NULL;
}

void CustomSprite::changeShaderProgram(bool useCustomShaderProgram)
{
}

void CustomSprite::sethasPremultipliedAlpha(int PremultipliedAlpha)
{
	_hasPremultipliedAlpha = PremultipliedAlpha;
}

bool CustomSprite::isCustomShaderProgramEnabled() const
{
	return _useCustomShaderProgram;
}

void CustomSprite::setColorBlendFunc(int colorBlendFuncNo)
{
	_colorBlendFuncNo = colorBlendFuncNo;
}

SSV3F_C4B_T2F_Quad& CustomSprite::getAttributeRef()
{
	return _sQuad;
}

void CustomSprite::setOpacity(unsigned char opacity)
{
	_opacity = static_cast<float>(opacity) / 255.0f;
}


#if 1
void CustomSprite::draw(void)
{
	/*
	*/
}
#endif

void CustomSprite::setFlippedX(bool flip)
{
	_flipX = flip;
}
void CustomSprite::setFlippedY(bool flip)
{
	_flipY = flip;
}
bool CustomSprite::isFlippedX()
{
	return (_flipX);
}
bool CustomSprite::isFlippedY()
{
	return (_flipY);
}

} //namespace ss
