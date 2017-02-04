// 
//  SS5Player.cpp
//
#include "SS5Player.h"
#include "SS5PlayerData.h"
#include "SS5PlayerTypes.h"
#include "common/Animator/ssplayer_matrix.h"
#include "player/ToPointer.h"
#include "player/DataArrayReader.h"
#include "player/Util.h"
#include "player/AnimeCache.h"
#include "player/CellCache.h"
#include "player/CustomSprite.h"
#include "player/EffectCache.h"
#include "player/State.h"
#include "player/ResourceSet.h"
#include "player/PlayerDef.h"
#include "ResluteState.h"
#include "ResourceManager.h"


namespace ss{


//座標回転処理
//指定した座標を中心に回転後した座標を取得します
void get_uv_rotation(float *u, float *v, float cu, float cv, float deg)
{
	float dx = *u - cu; // 中心からの距離(X)
	float dy = *v - cv; // 中心からの距離(Y)

	float tmpX = (dx * cosf(SSDegToRad(deg))) - (dy * sinf(SSDegToRad(deg))); // 回転
	float tmpY = (dx * sinf(SSDegToRad(deg))) + (dy * cosf(SSDegToRad(deg)));

	*u = (cu + tmpX); // 元の座標にオフセットする
	*v = (cv + tmpY);

}

//乱数シードに利用するユニークIDを作成します。
//この値は全てのSS5プレイヤー共通で使用します
int seedMakeID = 123456;
//エフェクトに与えるシードを取得する関数
unsigned int getRandomSeed()
{
	seedMakeID++;	//ユニークIDを更新します。
	//時間＋ユニークIDにする事で毎回シードが変わるようにします。
	unsigned int rc = (unsigned int)time(0) + (seedMakeID);

	return(rc);
}



/**
 * Player
 */

static const std::string s_nullString;

Player::Player(const ResourceSet *resource)
	: _currentRs(NULL)
	, _currentAnimeRef(NULL)
	, _frameSkipEnabled(true)
	, _playingFrame(0.0f)
	, _step(1.0f)
	, _loop(0)
	, _loopCount(0)
	, _isPlaying(false)
	, _isPausing(false)
	, _prevDrawFrameNo(-1)
	, _col_r(255)
	, _col_g(255)
	, _col_b(255)
	, _instanceOverWrite(false)
	, _motionBlendPlayer(NULL)
	, _blendTime(0.0f)
	, _blendTimeMax(0.0f)
	,_startFrameOverWrite(-1)	//開始フレームの上書き設定
	,_endFrameOverWrite(-1)		//終了フレームの上書き設定
	, _seedOffset(0)
{
	_currentRs = resource;
	SS_ASSERT_LOG(_currentRs, "resource is null");

	for (int i = 0; i < PART_VISIBLE_MAX; i++){
		_partVisible[i] = true;
		_partIndex[i] = -1;
		_cellChange[i] = -1;
	}
	_state.init();

	//ロードイベントを投げてcellMapのテクスチャを取得する
	int cellMapNum = _currentRs->m_cellCache->getCellMapNum();
	m_textures.resize(cellMapNum);
	for(int i = 0; i < cellMapNum; ++i){
		std::string textureName = _currentRs->m_cellCache->getTexturePath(i);
		//todo:イベントリスナーにしたい //TextureID textureid = m_eventListener->SSTextureLoad(textureName.c_str());	//ロードイベント
		//m_textures[i] = textureid;
		
	#if 0
		//CellCacheからこっちに持ってきた
		long tex = SSTextureLoad(path.c_str(), wrapmode, filtermode);
		SS_LOG("load: %s", path.c_str());
		TextuerData texdata;
		texdata.handle = tex;
		int w;
		int h;
		SSGetTextureSize(texdata.handle, w, h);
		texdata.size_w = w;
		texdata.size_h = h;
	#endif
		TextuerData& texdata = m_textures[i];
		texdata.handle = SSTextureLoad(textureName.c_str(), SsTexWrapMode::clamp, SsTexFilterMode::nearlest); // wrapmode, filtermode);//todo:wrapmode, filtermodeを引っ張ってくる。事前に取得できるようにする
		SSGetTextureSize(texdata.handle, texdata.size_w, texdata.size_h);
	}	
}

Player::~Player()
{
	if (_motionBlendPlayer)
	{
		delete (_motionBlendPlayer);
		_motionBlendPlayer = NULL;
	}

	releaseParts();

	//テクスチャの解放イベントを投げる
	for(TextuerData& texdata : m_textures){
		//todo:イベントリスナーにしたい	//m_eventListener->SSTextureRelease(textureid);
		SSTextureRelese(texdata.handle);
	}
	m_textures.clear();
}


int Player::getMaxFrame() const
{
	if (_currentAnimeRef )
	{
		return(_currentAnimeRef->m_animationData->numFrames);
	}
	else
	{
		return(0);
	}

}

int Player::getFrameNo() const
{
	return static_cast<int>(_playingFrame);
}

void Player::setFrameNo(int frameNo)
{
	_playingFrame = (float)frameNo;
}

float Player::getStep() const
{
	return _step;
}

void Player::setStep(float step)
{
	_step = step;
}

int Player::getLoop() const
{
	return _loop;
}

void Player::setLoop(int loop)
{
	if (loop < 0) return;
	_loop = loop;
}

int Player::getLoopCount() const
{
	return _loopCount;
}

void Player::clearLoopCount()
{
	_loopCount = 0;
}

void Player::setFrameSkipEnabled(bool enabled)
{
	_frameSkipEnabled = enabled;
	_playingFrame = (float)((int)_playingFrame);
}

bool Player::isFrameSkipEnabled() const
{
	return _frameSkipEnabled;
}


void Player::play(const std::string& ssaeName, const std::string& motionName, int loop, int startFrameNo)
{
	std::string animeName = ssaeName + "/" + motionName;
	play(animeName, loop, startFrameNo);
}

void Player::play(const std::string& animeName, int loop, int startFrameNo)
{
	SS_ASSERT_LOG(_currentRs != NULL, "Not select data");

	AnimeRef* animeRef = _currentRs->m_animeCache->getReference(animeName);
	SS_ASSERT_LOG(animeRef, "Not found animation > anime=%s", animeName.c_str());
	
	_currentAnimename = animeName;

	play(animeRef, loop, startFrameNo);
}

void Player::play(AnimeRef* animeRef, int loop, int startFrameNo)
{
	if (_currentAnimeRef != animeRef)
	{
		_currentAnimeRef = animeRef;
		
		allocParts(animeRef->m_numParts, false);
		setPartsParentage();
	}
	_playingFrame = static_cast<float>(startFrameNo);
	_step = 1.0f;
	_loop = loop;
	_loopCount = 0;
	_isPlaying = true;
	_isPausing = false;
	_prevDrawFrameNo = -1;
	_isPlayFirstUserdataChack = true;
	_animefps = _currentAnimeRef->m_animationData->fps;
	setStartFrame(-1);
	setEndFrame(-1);

	setFrame((int)_playingFrame);
}

//モーションブレンドしつつ再生
void Player::motionBlendPlay(const std::string& animeName, int loop, int startFrameNo, float blendTime)
{
	if (_currentAnimename != "")
	{
		//現在のアニメーションをブレンド用プレイヤーで再生
		if (_motionBlendPlayer == NULL)
		{
			_motionBlendPlayer = new Player(_currentRs);
		}
		int loopnum = _loop;
		if (_loop > 0)
		{
			loopnum = _loop - _loopCount;
		}
		_motionBlendPlayer->play(_currentAnimename, loopnum, getFrameNo());
		_motionBlendPlayer->setStep(_step);
		if (_loop > 0)
		{
			if (_loop == _loopCount)	//アニメは最後まで終了している
			{
				_motionBlendPlayer->animePause();
			}
		}
		_blendTime = 0;
		_blendTimeMax = blendTime;

	}
	play(animeName, loop, startFrameNo);

}



void Player::animePause()
{
	_isPausing = true;
}

void Player::animeResume()
{
	_isPausing = false;
}

void Player::stop()
{
	_isPlaying = false;
}

const std::string& Player::getPlayPackName() const
{
	return _currentAnimeRef != NULL ? _currentAnimeRef->m_packName : s_nullString;
}

const std::string& Player::getPlayAnimeName() const
{
	return _currentAnimeRef != NULL ? _currentAnimeRef->m_animeName : s_nullString;
}


void Player::update(float dt)
{
	updateFrame(dt);
}

void Player::updateFrame(float dt)
{
	if (!_currentAnimeRef) return;
	if (!_currentRs->m_data) return;

	int startFrame = 0;
	int endFrame = _currentAnimeRef->m_animationData->numFrames;
	if (_startFrameOverWrite != -1)
	{
		startFrame = _startFrameOverWrite;
	}
	if (_endFrameOverWrite != -1 )
	{ 
		endFrame = _endFrameOverWrite;
	}
	SS_ASSERT_LOG(startFrame < endFrame, "Playframe is out of range.");

	bool playEnd = false;
	bool toNextFrame = _isPlaying && !_isPausing;
	if (toNextFrame && (_loop == 0 || _loopCount < _loop))
	{
		// フレームを進める.
		// forward frame.
		const int numFrames = endFrame;

		float fdt = dt;
		float s = fdt / (1.0f / _currentAnimeRef->m_animationData->fps);
		
		//if (!m_frameSkipEnabled) SS_LOG("%f", s);
		
		float next = _playingFrame + (s * _step);

		int nextFrameNo = static_cast<int>(next);
		float nextFrameDecimal = next - static_cast<float>(nextFrameNo);
		int currentFrameNo = static_cast<int>(_playingFrame);

		//playを行って最初のupdateでは現在のフレームのユーザーデータを確認する
		if (_isPlayFirstUserdataChack == true)
		{
			checkUserData(currentFrameNo);
			_isPlayFirstUserdataChack = false;
		}

		if (_step >= 0)
		{
			// 順再生時.
			// normal plays.
			for (int c = nextFrameNo - currentFrameNo; c; c--)
			{
				int incFrameNo = currentFrameNo + 1;
				if (incFrameNo >= numFrames)
				{
					// アニメが一巡
					// turned animation.
					_loopCount += 1;
					if (_loop && _loopCount >= _loop)
					{
						// 再生終了.
						// play end.
						playEnd = true;
						break;
					}
					
					incFrameNo = startFrame;
					_seedOffset++;	//シードオフセットを加算
				}
				currentFrameNo = incFrameNo;

				// このフレームのユーザーデータをチェック
				// check the user data of this frame.
				checkUserData(currentFrameNo);
			}
		}
		else
		{
			// 逆再生時.
			// reverse play.
			for (int c = currentFrameNo - nextFrameNo; c; c--)
			{
				int decFrameNo = currentFrameNo - 1;
				if (decFrameNo < startFrame)
				{
					// アニメが一巡
					// turned animation.
					_loopCount += 1;
					if (_loop && _loopCount >= _loop)
					{
						// 再生終了.
						// play end.
						playEnd = true;
						break;
					}
				
					decFrameNo = numFrames - 1;
					_seedOffset++;	//シードオフセットを加算
				}
				currentFrameNo = decFrameNo;
				
				// このフレームのユーザーデータをチェック
				// check the user data of this frame.
				checkUserData(currentFrameNo);
			}
		}
		
		_playingFrame = static_cast<float>(currentFrameNo) + nextFrameDecimal;


	}
	else
	{
		//アニメを手動で更新する場合
		checkUserData(getFrameNo());
	}
	//モーションブレンド用アップデート
	if (_motionBlendPlayer)
	{
		_motionBlendPlayer->update(dt);
		_blendTime = _blendTime + dt;
		if (_blendTime >= _blendTimeMax)
		{
			_blendTime = _blendTimeMax;
			//プレイヤーを削除する
			delete (_motionBlendPlayer);
			_motionBlendPlayer = NULL;
		}
	}

	setFrame(getFrameNo(), dt);
	
	if (playEnd)
	{
		stop();
	
		// 再生終了コールバックの呼び出し
		SSPlayEnd(this);
	}
}


void Player::allocParts(int numParts, bool useCustomShaderProgram)
{
	releaseParts();	//すべてのパーツを消す

	// パーツ数だけCustomSpriteを作成する
	for (int i = 0; i < numParts; i++){
		CustomSprite* sprite =  CustomSprite::create();
		sprite->_ssplayer = NULL;
		sprite->changeShaderProgram(useCustomShaderProgram);

		_parts.push_back(sprite);
	}
}

void Player::releaseParts()
{
	SS_ASSERT(_currentRs);
	SS_ASSERT(_currentAnimeRef);

	// パーツの子CustomSpriteを全て削除
	for(CustomSprite* sprite : _parts){
		SS_SAFE_DELETE(sprite->_ssplayer);	//todo:customspriteがやるべき
		SS_SAFE_DELETE(sprite);
	}
	_parts.clear();
}

void Player::setPartsParentage()
{
	if (!_currentAnimeRef) return;

	ToPointer ptr(_currentRs->m_data);
	int numParts = _currentAnimeRef->m_numParts;
	
	//親子関係を設定
	for (int partIndex = 0; partIndex < numParts; partIndex++)
	{
		const PartData* partData = _currentAnimeRef->getPartData(partIndex);
		CustomSprite* sprite = static_cast<CustomSprite*>(_parts.at(partIndex));
		
		if (partIndex > 0)
		{
			CustomSprite* parent = static_cast<CustomSprite*>(_parts.at(partData->parentIndex));
			sprite->_parent = parent;
		}
		else
		{
			sprite->_parent = NULL;
		}

		//インスタンスパーツの生成
		std::string refanimeName = ptr.toString(partData->refname);

		SS_SAFE_DELETE(sprite->_ssplayer);
		if (refanimeName != "")
		{
			//インスタンスパーツが設定されている
			sprite->_ssplayer = new Player(_currentRs);
			sprite->_ssplayer->play(refanimeName);				 // アニメーション名を指定(ssae名/アニメーション名も可能、詳しくは後述)
			sprite->_ssplayer->animePause();
		}

		//エフェクトパーツの生成
		if (sprite->refEffect)
		{
			delete sprite->refEffect;
			sprite->refEffect = 0;
		}

		std::string refeffectName = ptr.toString(partData->effectfilename);
		if (refeffectName != "")
		{
			SsEffectModel* effectmodel = _currentRs->m_effectCache->getReference(refeffectName);
			if (effectmodel)
			{
				//エフェクトクラスにパラメータを設定する
				SsEffectRenderV2* er = new SsEffectRenderV2();
				sprite->refEffect = er;
				sprite->refEffect->setParentAnimeState(&sprite->partState);
				sprite->refEffect->setEffectData(effectmodel);
//				sprite->refEffect->setEffectSprite(&_effectSprite);	//エフェクトクラスに渡す都合上publicにしておく
//				sprite->refEffect->setEffectSpriteCount(&_effectSpriteCount);	//エフェクトクラスに渡す都合上publicにしておく
				sprite->refEffect->setSeed(getRandomSeed());
				sprite->refEffect->reload();
				sprite->refEffect->stop();
				sprite->refEffect->setLoop(false);
			}
		}
	}
}

//再生しているアニメーションに含まれるパーツ数を取得
int Player::getPartsCount()
{
	return _currentAnimeRef->m_numParts;
}

//indexからパーツ名を取得
const char* Player::getPartName(int partId) const
{
	ToPointer ptr(_currentRs->m_data);
	SS_ASSERT_LOG(partId >= 0 && partId < _currentAnimeRef->m_numParts, "partId is out of range.");

	const PartData* partData = _currentAnimeRef->m_partDatas;
	const char* name = ptr.toString(partData[partId].name);
	return name;
}

//パーツ名からindexを取得
int Player::indexOfPart(const char* partName) const
{
	for (int i = 0; i < _currentAnimeRef->m_numParts; i++){
		const char* name = getPartName(i);
		if (strcmp(partName, name) == 0){
			return i;
		}
	}
	return -1;
}

/*
 パーツ名から指定フレームのパーツステータスを取得します。
 必要に応じて　ResluteState　を編集しデータを取得してください。

 指定したフレームの状態にすべてのパーツのステータスを更新します。
 描画を行う前にupdateを呼び出し、パーツステータスを表示に状態に戻してからdrawしてください。
*/
bool Player::getPartState(ResluteState& result, const char* name, int frameNo)
{
	bool rc = false;
	if (_currentAnimeRef)
	{
		{
			//カレントフレームのパーツステータスを取得する
			if (frameNo == -1)
			{
				//フレームの指定が省略された場合は現在のフレームを使用する
				frameNo = getFrameNo();
			}

			if (frameNo != getFrameNo())
			{
				//取得する再生フレームのデータが違う場合プレイヤーを更新する
				//パーツステータスの更新
				setFrame(frameNo);
			}

			ToPointer ptr(_currentRs->m_data);

			for (int index = 0; index < _currentAnimeRef->m_numParts; index++)
			{
				int partIndex = _partIndex[index];

				const PartData* partData = _currentAnimeRef->getPartData(partIndex);
				const char* partName = ptr.toString(partData->name);
				if (strcmp(partName, name) == 0)
				{
					//必要に応じて取得するパラメータを追加してください。
					//当たり判定などのパーツに付属するフラグを取得する場合は　partData　のメンバを参照してください。
					//親から継承したスケールを反映させる場合はxスケールは_mat.m[0]、yスケールは_mat.m[5]をかけて使用してください。
					CustomSprite* sprite = static_cast<CustomSprite*>(_parts.at(partIndex));
					result.x = sprite->_state.mat[12];
					result.y = sprite->_state.mat[13];

					//パーツアトリビュート
//					sprite->_state;												//SpriteStudio上のアトリビュートの値は_stateから取得してください
					result.flags = sprite->_state.flags;						// このフレームで更新が行われるステータスのフラグ
					result.cellIndex = sprite->_state.cellIndex;				// パーツに割り当てられたセルの番号
					result.x = sprite->_state.mat[12];
					result.y = sprite->_state.mat[13];
					result.z = sprite->_state.z;
					result.pivotX = sprite->_state.pivotX;						// 原点Xオフセット＋セルに設定された原点オフセットX
					result.pivotY = sprite->_state.pivotY;						// 原点Yオフセット＋セルに設定された原点オフセットY
					result.rotationX = sprite->_state.rotationX;				// X回転（親子関係計算済）
					result.rotationY = sprite->_state.rotationY;				// Y回転（親子関係計算済）
					result.rotationZ = sprite->_state.rotationZ;				// Z回転（親子関係計算済）
					result.scaleX = sprite->_state.scaleX;						// Xスケール（親子関係計算済）
					result.scaleY = sprite->_state.scaleY;						// Yスケール（親子関係計算済）
					result.opacity = sprite->_state.opacity;					// 不透明度（0～255）（親子関係計算済）
					result.size_X = sprite->_state.size_X;						// SS5アトリビュート：Xサイズ
					result.size_Y = sprite->_state.size_Y;						// SS5アトリビュート：Xサイズ
					result.uv_move_X = sprite->_state.uv_move_X;				// SS5アトリビュート：UV X移動
					result.uv_move_Y = sprite->_state.uv_move_Y;				// SS5アトリビュート：UV Y移動
					result.uv_rotation = sprite->_state.uv_rotation;			// SS5アトリビュート：UV 回転
					result.uv_scale_X = sprite->_state.uv_scale_X;				// SS5アトリビュート：UV Xスケール
					result.uv_scale_Y = sprite->_state.uv_scale_Y;				// SS5アトリビュート：UV Yスケール
					result.boundingRadius = sprite->_state.boundingRadius;		// SS5アトリビュート：当たり半径
					result.colorBlendFunc = sprite->_state.colorBlendFunc;		// SS5アトリビュート：カラーブレンドのブレンド方法
					result.colorBlendType = sprite->_state.colorBlendType;		// SS5アトリビュート：カラーブレンドの単色か頂点カラーか。
					result.flipX = sprite->_state.flipX;						// 横反転（親子関係計算済）
					result.flipY = sprite->_state.flipY;						// 縦反転（親子関係計算済）
					result.isVisibled = sprite->_state.isVisibled;				// 非表示（親子関係計算済）

					//パーツ設定
					result.part_type = partData->type;							//パーツ種別
					result.part_boundsType = partData->boundsType;				//当たり判定種類
					result.part_alphaBlendType = partData->alphaBlendType;		// BlendType
					//ラベルカラー
					std::string colorName = ptr.toString(partData->colorLabel);
					if (colorName == COLORLABELSTR_NONE)
					{
						result.part_labelcolor = COLORLABEL_NONE;
					}
					if (colorName == COLORLABELSTR_RED)
					{
						result.part_labelcolor = COLORLABEL_RED;
					}
					if (colorName == COLORLABELSTR_ORANGE)
					{
						result.part_labelcolor = COLORLABEL_ORANGE;
					}
					if (colorName == COLORLABELSTR_YELLOW)
					{
						result.part_labelcolor = COLORLABEL_YELLOW;
					}
					if (colorName == COLORLABELSTR_GREEN)
					{
						result.part_labelcolor = COLORLABEL_GREEN;
					}
					if (colorName == COLORLABELSTR_BLUE)
					{
						result.part_labelcolor = COLORLABEL_BLUE;
					}
					if (colorName == COLORLABELSTR_VIOLET)
					{
						result.part_labelcolor = COLORLABEL_VIOLET;
					}
					if (colorName == COLORLABELSTR_GRAY)
					{
						result.part_labelcolor = COLORLABEL_GRAY;
					}

					rc = true;
					break;
				}
			}
			//パーツステータスを表示するフレームの内容で更新
			if (frameNo != getFrameNo())
			{
				//取得する再生フレームのデータが違う場合プレイヤーの状態をもとに戻す
				//パーツステータスの更新
				setFrame(getFrameNo());
			}
		}
	}
	return rc;
}


//ラベル名からラベルの設定されているフレームを取得
//ラベルが存在しない場合は戻り値が-1となります。
//ラベル名が全角でついていると取得に失敗します。
int Player::getLabelToFrame(char* findLabelName)
{
	int rc = -1;

	ToPointer ptr(_currentRs->m_data);
	const AnimationData* animeData = _currentAnimeRef->m_animationData;

	if (!animeData->labelData) return -1;
	const ss_offset* labelDataIndex = static_cast<const ss_offset*>(ptr(animeData->labelData));


	int idx = 0;
	for (idx = 0; idx < animeData->labelNum; idx++ )
	{
		if (!labelDataIndex[idx]) return -1;
		const ss_u16* labelDataArray = static_cast<const ss_u16*>(ptr(labelDataIndex[idx]));

		DataArrayReader reader(labelDataArray);

		LabelData ldata;
		ss_offset offset = reader.readOffset();
		const char* str = ptr.toString(offset);
		int labelFrame = reader.readU16();
		ldata.str = str;
		ldata.frameNo = labelFrame;

		if (ldata.str.compare(findLabelName) == 0 )
		{
			//同じ名前のラベルが見つかった
			return (ldata.frameNo);
		}
	}

	return (rc);
}

//特定パーツの表示、非表示を設定します
//パーツ番号はスプライトスタジオのフレームコントロールに配置されたパーツが
//プライオリティでソートされた後、上に配置された順にソートされて決定されます。
void Player::setPartVisible(std::string partsname, bool flg)
{
	int index = indexOfPart(partsname.c_str());
	if(index >= 0){
		_partVisible[index] = flg;
	}
}

//パーツに割り当たるセルを変更します
void Player::setPartCell(std::string partsname, std::string sscename, std::string cellname)
{
	bool rc = false;
	if (_currentAnimeRef)
	{
		ToPointer ptr(_currentRs->m_data);

		int changeCellIndex = -1;
		if ((sscename != "") && (cellname != ""))
		{
			//セルマップIDを取得する
			const Cell* cells = ptr.toCells(_currentRs->m_data);

			//名前からインデックスの取得
			int cellindex = -1;
			for (int i = 0; i < _currentRs->m_data->numCells; i++)
			{
				const Cell* cell = &cells[i];
				const char* name1 = ptr.toString(cell->name);
				const CellMap* cellMap = ptr.toCellMap(cell);
				const char* name2 = ptr.toString(cellMap->name);
				if (strcmp(cellname.c_str(), name1) == 0)
				{
					if (strcmp(sscename.c_str(), name2) == 0)
					{
						changeCellIndex = i;
						break;
					}
				}
			}
		}

		for (int index = 0; index < _currentAnimeRef->m_numParts; index++)
		{
			int partIndex = _partIndex[index];

			const PartData* partData = _currentAnimeRef->getPartData(partIndex);
			const char* partName = ptr.toString(partData->name);
			if (strcmp(partName, partsname.c_str()) == 0)
			{
				//セル番号を設定
				_cellChange[index] = changeCellIndex;	//上書き解除
				break;
			}
		}
	}
}

// インスタンスパーツが再生するアニメを変更します。
bool Player::changeInstanceAnime(std::string partsname, std::string animename, bool overWrite, Instance keyParam)
{
	//名前からパーツを取得
	bool rc = false;
	if (_currentAnimeRef)
	{
		ToPointer ptr(_currentRs->m_data);

		for (int index = 0; index < _currentAnimeRef->m_numParts; index++)
		{
			int partIndex = _partIndex[index];

			const PartData* partData = _currentAnimeRef->getPartData(partIndex);
			const char* partName = ptr.toString(partData->name);
			if (strcmp(partName, partsname.c_str()) == 0)
			{
				CustomSprite* sprite = static_cast<CustomSprite*>(_parts.at(partIndex));
				if (sprite->_ssplayer)
				{
					//パーツがインスタンスパーツの場合は再生するアニメを設定する
					//アニメが入れ子にならないようにチェックする
					if (_currentAnimename != animename)
					{
						sprite->_ssplayer->play(animename);
						sprite->_ssplayer->setInstanceParam(overWrite, keyParam);	//インスタンスパラメータの設定
						sprite->_ssplayer->animeResume();		//アニメ切り替え時にがたつく問題の対応
						sprite->_liveFrame = 0;					//独立動作の場合再生位置をリセット
						rc = true;
					}
				}

				break;
			}
		}
	}

	return (rc);
}
//インスタンスパラメータを設定します
void Player::setInstanceParam(bool overWrite, Instance keyParam)
{
	_instanceOverWrite = overWrite;		//インスタンス情報を上書きするか？
	_instanseParam = keyParam;			//インスタンスパラメータ

}
//インスタンスパラメータを取得します
void Player::getInstanceParam(bool *overWrite, Instance *keyParam)
{
	*overWrite = _instanceOverWrite;		//インスタンス情報を上書きするか？
	*keyParam = _instanseParam;			//インスタンスパラメータ
}

//アニメーションの色成分を変更します
void Player::setColor(int r, int g, int b)
{
	_col_r = r;
	_col_g = g;
	_col_b = b;
}

//アニメーションのループ範囲を設定します
void Player::setStartFrame(int frame)
{
	_startFrameOverWrite = frame;	//開始フレームの上書き設定
	//現在フレームより後の場合は先頭フレームに設定する
	if (getFrameNo() < frame)
	{
		setFrameNo(frame);
	}
}
void Player::setEndFrame(int frame)
{
	_endFrameOverWrite = frame;		//終了フレームの上書き設定
}
//アニメーションのループ範囲をラベル名で設定します
void Player::setStartFrameToLabelName(char *findLabelName)
{
	int frame = getLabelToFrame(findLabelName);
	setStartFrame(frame);
}
void Player::setEndFrameToLabelName(char *findLabelName)
{
	int frame = getLabelToFrame(findLabelName);
	if (frame != -1)
	{
		frame += 1;
	}
	setEndFrame(frame);
}

//スプライト情報の取得
CustomSprite* Player::getSpriteData(int partIndex)
{
	CustomSprite* sprite = NULL;
	if (_parts.size() < partIndex)
	{
	}
	else
	{
		sprite = static_cast<CustomSprite*>(_parts.at(partIndex));
	}
	return(sprite);
}

/*
* 表示を行うパーツ数を取得します
*/
int Player::getDrawSpriteCount(void)
{
	return (_draw_count);
}

void Player::setFrame(int frameNo, float dt)
{
	if (!_currentAnimeRef) return;
	if (!_currentRs->m_data) return;

	bool forceUpdate = false;
	{
		// フリップに変化があったときは必ず描画を更新する
		CustomSprite* root = static_cast<CustomSprite*>(_parts.at(0));
		float scaleX = root->isFlippedX() ? -1.0f : 1.0f;
		float scaleY = root->isFlippedY() ? -1.0f : 1.0f;
		root->setStateValue(root->_state.x, scaleX);
		root->setStateValue(root->_state.y, scaleY);
		forceUpdate = root->_isStateChanged;
	}
	
	// 前回の描画フレームと同じときはスキップ
	//インスタンスアニメがあるので毎フレーム更新するためコメントに変更
	//	if (!forceUpdate && frameNo == _prevDrawFrameNo) return;

	ToPointer ptr(_currentRs->m_data);
	const AnimationData* animeData = _currentAnimeRef->m_animationData;
	const ss_offset* frameDataIndex = static_cast<const ss_offset*>(ptr(animeData->frameData));
	
	const ss_u16* frameDataArray = static_cast<const ss_u16*>(ptr(frameDataIndex[frameNo]));
	DataArrayReader reader(frameDataArray);
	
	const AnimationInitialData* initialDataList = ptr.toAnimationInitialDatas(animeData);


	for (int index = 0; index < _currentAnimeRef->m_numParts; index++){

		int partIndex = reader.readS16();
		const PartData* partData = _currentAnimeRef->getPartData(partIndex);
		const AnimationInitialData* init = &initialDataList[partIndex];

		State state;
		state.readData(reader, init);

		//ユーザーが任意に非表示としたパーツは非表示に設定
		if (_partVisible[index] == false){
			state.isVisibled = false;					//todo:これは描画のときに見ればいいはず
		}
		//ユーザーがセルを上書きした
		if (_cellChange[index] != -1){
			state.cellIndex = _cellChange[index];
		}

		_partIndex[index] = partIndex;

		//プレイヤーのフリップ
		if ( _state.flipX == true ){
			state.flipX = !state.flipX;	//フラグ反転		//todo:全体を反転するのはスケール==-1で十分なはず
		}
		if (_state.flipY == true){
			state.flipY = !state.flipY;	//フラグ反転		//todo:全体を反転するのはスケール==-1で十分なはず
		}

		//セルの原点設定を反映させる
		const CellRef* cellRef = state.cellIndex >= 0 ? _currentRs->m_cellCache->getReference(state.cellIndex) : nullptr;
		if (cellRef)
		{
			float cpx = 0;
			float cpy = 0;

			cpx = cellRef->m_cell->pivot_X;
			if (state.flipX) cpx = -cpx;	// 水平フリップによって原点を入れ替える
			cpy = cellRef->m_cell->pivot_Y;
			if (state.flipY) cpy = -cpy;	// 垂直フリップによって原点を入れ替える

			state.pivotX += cpx;
			state.pivotY += cpy;

		}
		state.pivotX += 0.5f;
		state.pivotY += 0.5f;

		//モーションブレンド
		if (_motionBlendPlayer)
		{
			CustomSprite* blendSprite = _motionBlendPlayer->getSpriteData(partIndex);
			if (blendSprite)
			{ 
				float percent = _blendTime / _blendTimeMax;
				state.x = parcentVal(state.x, blendSprite->_orgState.x, percent);
				state.y = parcentVal(state.y, blendSprite->_orgState.y, percent);
				state.scaleX = parcentVal(state.scaleX, blendSprite->_orgState.scaleX, percent);
				state.scaleY = parcentVal(state.scaleY, blendSprite->_orgState.scaleY, percent);
				state.rotationX = parcentValRot(state.rotationX, blendSprite->_orgState.rotationX, percent);
				state.rotationY = parcentValRot(state.rotationY, blendSprite->_orgState.rotationY, percent);
				state.rotationZ = parcentValRot(state.rotationZ, blendSprite->_orgState.rotationZ, percent);
			}

		}

		CustomSprite* sprite = static_cast<CustomSprite*>(_parts.at(partIndex));

		//反転
		//反転はUVにも反映させておくので使いやすい方で反転してください。
		sprite->setFlippedX(state.flipX);
		sprite->setFlippedY(state.flipY);

		bool setBlendEnabled = true;

		if (cellRef)
		{
			//各パーツのテクスチャ情報を設定
			state.texture = m_textures[cellRef->m_cellMapIndex]; //cellRef->m_texture;
			state.rect = cellRef->m_rect;
			state.blendfunc = partData->alphaBlendType;

			if (setBlendEnabled)
			{
				if (state.flags & PART_FLAG_COLOR_BLEND)
				{
					//カラーブレンドを行うときはカスタムシェーダーを使用する
					sprite->changeShaderProgram(true);
				}
				else
				{
					sprite->changeShaderProgram(false);
				}
			}
		}
		else
		{
			state.texture.handle = -1;
			//セルが無く通常パーツ、ヌルパーツの時は非表示にする
			if ((partData->type == PARTTYPE_NORMAL) || (partData->type == PARTTYPE_NULL))
			{
				state.isVisibled = false;
			}
		}
		sprite->setOpacity(state.opacity);

		//頂点データの設定
		//quadにはプリミティブの座標（頂点変形を含む）、UV、カラー値が設定されます。
		SSV3F_C4B_T2F_Quad quad;
		memset(&quad, 0, sizeof(quad));
		if (cellRef)
		{
			//頂点を設定する
			float width_h = cellRef->m_rect.width() / 2;
			float height_h = cellRef->m_rect.height() / 2;
			float x1 = -width_h;
			float y1 = -height_h;
			float x2 = width_h;
			float y2 = height_h;

#ifdef UP_MINUS
			quad.tl.vertices.x = x1;
			quad.tl.vertices.y = y1;
			quad.tr.vertices.x = x2;
			quad.tr.vertices.y = y1;
			quad.bl.vertices.x = x1;
			quad.bl.vertices.y = y2;
			quad.br.vertices.x = x2;
			quad.br.vertices.y = y2;
#else
			quad.tl.vertices.x = x1;
			quad.tl.vertices.y = y2;
			quad.tr.vertices.x = x2;
			quad.tr.vertices.y = y2;
			quad.bl.vertices.x = x1;
			quad.bl.vertices.y = y1;
			quad.br.vertices.x = x2;
			quad.br.vertices.y = y1;
#endif
		}

		//サイズ設定
		//頂点をサイズに合わせて変形させる
		if (state.flags & PART_FLAG_SIZE_X)
		{
			float w = 0;
			float center = 0;
			w = (quad.tr.vertices.x - quad.tl.vertices.x) / 2.0f;
			if (w!= 0.0f)
			{
				center = quad.tl.vertices.x + w;
				float scale = (state.size_X / 2.0f) / w;

				quad.bl.vertices.x = center - (w * scale);
				quad.br.vertices.x = center + (w * scale);
				quad.tl.vertices.x = center - (w * scale);
				quad.tr.vertices.x = center + (w * scale);
			}
		}
		if (state.flags & PART_FLAG_SIZE_Y)
		{
			float h = 0;
			float center = 0;
			h = (quad.bl.vertices.y - quad.tl.vertices.y) / 2.0f;
			if (h != 0.0f)
			{
				center = quad.tl.vertices.y + h;
				float scale = (state.size_Y / 2.0f) / h;

				quad.bl.vertices.y = center - (h * scale);
				quad.br.vertices.y = center - (h * scale);
				quad.tl.vertices.y = center + (h * scale);
				quad.tr.vertices.y = center + (h * scale);
			}
		}
		// 頂点変形のオフセット値を反映
		if (state.flags & PART_FLAG_VERTEX_TRANSFORM)
		{
			int vt_flags = reader.readU16();
			if (vt_flags & VERTEX_FLAG_LT)
			{
				quad.tl.vertices.x += reader.readS16();
				quad.tl.vertices.y += reader.readS16();
			}
			if (vt_flags & VERTEX_FLAG_RT)
			{
				quad.tr.vertices.x += reader.readS16();
				quad.tr.vertices.y += reader.readS16();
			}
			if (vt_flags & VERTEX_FLAG_LB)
			{
				quad.bl.vertices.x += reader.readS16();
				quad.bl.vertices.y += reader.readS16();
			}
			if (vt_flags & VERTEX_FLAG_RB)
			{
				quad.br.vertices.x += reader.readS16();
				quad.br.vertices.y += reader.readS16();
			}
		}
		
		//頂点情報の取得
		unsigned char alpha = (unsigned char)state.opacity;
		SSColor4B color4 = { 0xff, 0xff, 0xff, 0xff };

		color4.r = color4.r * _col_r / 255;
		color4.g = color4.g * _col_g / 255;
		color4.b = color4.b * _col_b / 255;

		quad.tl.colors =
		quad.tr.colors =
		quad.bl.colors =
		quad.br.colors = color4;


		// カラーブレンドの反映
		if (state.flags & PART_FLAG_COLOR_BLEND){

			int typeAndFlags = reader.readU16();
			int funcNo = typeAndFlags & 0xff;
			int cb_flags = (typeAndFlags >> 8) & 0xff;
			float blend_rate = 1.0f;

			sprite->setColorBlendFunc(funcNo);
			sprite->_state.colorBlendFunc = funcNo;
			sprite->_state.colorBlendType = cb_flags;

			//ssbpではカラーブレンドのレート（％）は使用できません。
			//制限となります。
			if (cb_flags & VERTEX_FLAG_ONE){
				color4.readColorWithRate(reader);

				color4.r = color4.r * _col_r / 255;
				color4.g = color4.g * _col_g / 255;
				color4.b = color4.b * _col_b / 255;
				color4.a = color4.a * alpha / 255;

				quad.tl.colors =
				quad.tr.colors =
				quad.bl.colors =
				quad.br.colors = color4;
			}
			else{
				if (cb_flags & VERTEX_FLAG_LT){
					quad.tl.colors.readColorWithRate(reader);
				}
				if (cb_flags & VERTEX_FLAG_RT){
					quad.tr.colors.readColorWithRate(reader);
				}
				if (cb_flags & VERTEX_FLAG_LB){
					quad.bl.colors.readColorWithRate(reader);
				}
				if (cb_flags & VERTEX_FLAG_RB){
					quad.br.colors.readColorWithRate(reader);
				}
			}
		}


		//UVを設定する
		SSTex2F uv_tl, uv_br;
		if(cellRef){
			uv_tl = SSTex2F(cellRef->m_cell->u1, cellRef->m_cell->v1);
			uv_br = SSTex2F(cellRef->m_cell->u2, cellRef->m_cell->v2);
		}
		state.uvCompute(&quad, uv_tl, uv_br);

		state.quad = quad;




		//インスタンスパーツの場合
		if (partData->type == PARTTYPE_INSTANCE)
		{
			bool overWrite;
			Instance keyParam;
			sprite->_ssplayer->getInstanceParam(&overWrite, &keyParam);
			//描画
			int refKeyframe = state.instanceValue_curKeyframe;
			int refStartframe = state.instanceValue_startFrame;
			int refEndframe = state.instanceValue_endFrame;
			float refSpeed = state.instanceValue_speed;
			int refloopNum = state.instanceValue_loopNum;
			bool infinity = false;
			bool reverse = false;
			bool pingpong = false;
			bool independent = false;

			int lflags = state.instanceValue_loopflag;
			if (lflags & INSTANCE_LOOP_FLAG_INFINITY )
			{
				//無限ループ
				infinity = true;
			}
			if (lflags & INSTANCE_LOOP_FLAG_REVERSE)
			{
				//逆再生
				reverse = true;
			}
			if (lflags & INSTANCE_LOOP_FLAG_PINGPONG)
			{
				//往復
				pingpong = true;
			}
			if (lflags & INSTANCE_LOOP_FLAG_INDEPENDENT)
			{
				//独立
				independent = true;
			}
			//インスタンスパラメータを上書きする
			if (overWrite == true)
			{
				refStartframe = keyParam.refStartframe;		//開始フレーム
				refEndframe = keyParam.refEndframe;			//終了フレーム
				refSpeed = keyParam.refSpeed;				//再生速度
				refloopNum = keyParam.refloopNum;			//ループ回数
				infinity = keyParam.infinity;				//無限ループ
				reverse = keyParam.reverse;					//逆再選
				pingpong = keyParam.pingpong;				//往復
				independent = keyParam.independent;			//独立動作
			}

			//タイムライン上の時間 （絶対時間）
			int time = frameNo;

			//独立動作の場合
			if (independent)
			{
				float delta = dt / (1.0f / _animefps);						//	独立動作時は親アニメのfpsを使用する
//				float delta = fdt / (1.0f / sprite->_ssplayer->_animefps);

				sprite->_liveFrame += delta;
				time = (int)sprite->_liveFrame;
			}

			//このインスタンスが配置されたキーフレーム（絶対時間）
			int	selfTopKeyframe = refKeyframe;


			int	reftime = (int)((float)(time - selfTopKeyframe) * refSpeed); //開始から現在の経過時間
			if (reftime < 0) continue;							//そもそも生存時間に存在していない
			if (selfTopKeyframe > time) continue;

			int inst_scale = (refEndframe - refStartframe) + 1; //インスタンスの尺


			//尺が０もしくはマイナス（あり得ない
			if (inst_scale <= 0) continue;
			int	nowloop = (reftime / inst_scale);	//現在までのループ数

			int checkloopnum = refloopNum;

			//pingpongの場合では２倍にする
			if (pingpong) checkloopnum = checkloopnum * 2;

			//無限ループで無い時にループ数をチェック
			if (!infinity)   //無限フラグが有効な場合はチェックせず
			{
				if (nowloop >= checkloopnum)
				{
					reftime = inst_scale - 1;
					nowloop = checkloopnum - 1;
				}
			}

			int temp_frame = reftime % inst_scale;  //ループを加味しないインスタンスアニメ内のフレーム

			//参照位置を決める
			//現在の再生フレームの計算
			int _time = 0;
			if (pingpong && (nowloop % 2 == 1))
			{
				if (reverse)
				{
					reverse = false;//反転
				}
				else
				{
					reverse = true;//反転
				}
			}

			if (reverse)
			{
				//リバースの時
				_time = refEndframe - temp_frame;
			}
			else{
				//通常時
				_time = temp_frame + refStartframe;
			}

			//インスタンスパラメータを設定
			sprite->_ssplayer->setColor(_col_r, _col_g, _col_b);

			//インスタンス用SSPlayerに再生フレームを設定する
			sprite->_ssplayer->setFrameNo(_time);
		}

		//スプライトステータスの保存
		sprite->setState(state);
		sprite->_orgState = sprite->_state;

	}


	// 親に変更があるときは自分も更新するようフラグを設定する
	for (int partIndex = 1; partIndex < _currentAnimeRef->m_numParts; partIndex++)
	{
		const PartData* partData = _currentAnimeRef->getPartData(partIndex);
		CustomSprite* sprite = static_cast<CustomSprite*>(_parts.at(partIndex));
		CustomSprite* parent = static_cast<CustomSprite*>(_parts.at(partData->parentIndex));
		
		if (parent->_isStateChanged)
		{
			sprite->_isStateChanged = true;
		}
	}

	// 行列の更新
	float mat[16];
	float t[16];
	for (int partIndex = 0; partIndex < _currentAnimeRef->m_numParts; partIndex++)
	{
		const PartData* partData = _currentAnimeRef->getPartData(partIndex);
		CustomSprite* sprite = static_cast<CustomSprite*>(_parts.at(partIndex));

		if (sprite->_isStateChanged){
			
			if (partIndex > 0)
			{
				//親のマトリクスを適用
				CustomSprite* parent = static_cast<CustomSprite*>(_parts.at(partData->parentIndex));
				memcpy(mat, parent->_mat, sizeof(float) * 16);
			}
			else
			{
				IdentityMatrix(mat);
				//rootパーツはプレイヤーからステータスを引き継ぐ
				sprite->_state.x += _state.x;
				sprite->_state.y += _state.y;
				sprite->_state.rotationX += _state.rotationX;
				sprite->_state.rotationY += _state.rotationY;
				sprite->_state.rotationZ += _state.rotationZ;
				sprite->_state.scaleX *= _state.scaleX;
				sprite->_state.scaleY *= _state.scaleY;
				//プレイヤーのフリップ
				if (_state.flipX == true)
				{
					sprite->_state.scaleX = -sprite->_state.scaleX;	//フラグ反転
				}
				if (_state.flipY == true)
				{
					sprite->_state.scaleY = -sprite->_state.scaleY;	//フラグ反転
				}

				sprite->_state.Calc_rotationX = sprite->_state.rotationX;
				sprite->_state.Calc_rotationY = sprite->_state.rotationY;
				sprite->_state.Calc_rotationZ = sprite->_state.rotationZ;

				sprite->_state.Calc_scaleX = sprite->_state.scaleX;
				sprite->_state.Calc_scaleY = sprite->_state.scaleY;
			}
			TranslationMatrix(t, sprite->_state.x, sprite->_state.y, 0.0f);
			MultiplyMatrix(t, mat, mat);

			Matrix4RotationX(t, SSDegToRad(sprite->_state.rotationX));
			MultiplyMatrix(t, mat, mat);

			Matrix4RotationY(t, SSDegToRad(sprite->_state.rotationY));
			MultiplyMatrix(t, mat, mat);

			Matrix4RotationZ(t, SSDegToRad(sprite->_state.rotationZ));
			MultiplyMatrix(t, mat, mat);

			ScaleMatrix(t, sprite->_state.scaleX, sprite->_state.scaleY, 1.0f);
			MultiplyMatrix(t, mat, mat);

			memcpy(sprite->_mat, mat, sizeof(float) * 16);
			memcpy(sprite->_state.mat, mat, sizeof(float) * 16);

			if (partIndex > 0)
			{
				CustomSprite* parent = static_cast<CustomSprite*>(_parts.at(partData->parentIndex));
				//子供は親のステータスを引き継ぐ
				//座標はマトリクスから取得する
				if ((parent->_state.Calc_scaleX * parent->_state.Calc_scaleY) < 0)	//スケールのどちらかが-の場合は回転方向を逆にする
				{
					sprite->_state.Calc_rotationZ = -sprite->_state.Calc_rotationZ;
				}
				sprite->_state.Calc_rotationX += parent->_state.Calc_rotationX;
				sprite->_state.Calc_rotationY += parent->_state.Calc_rotationY;
				sprite->_state.Calc_rotationZ += parent->_state.Calc_rotationZ;

				sprite->_state.Calc_scaleX *= parent->_state.Calc_scaleX;
				sprite->_state.Calc_scaleY *= parent->_state.Calc_scaleY;

				//ルートパーツのアルファ値を反映させる
				sprite->_state.Calc_opacity = (sprite->_state.Calc_opacity * _state.opacity) / 255;
				//インスタンスパーツの親を設定
				if (sprite->_ssplayer)
				{
					sprite->_ssplayer->setPosition(sprite->_mat[12], sprite->_mat[13]);
					sprite->_ssplayer->setScale(sprite->_state.Calc_scaleX, sprite->_state.Calc_scaleY);
					sprite->_ssplayer->setRotation(sprite->_state.Calc_rotationX, sprite->_state.Calc_rotationY, sprite->_state.Calc_rotationZ);
					sprite->_ssplayer->setAlpha(sprite->_state.Calc_opacity);
				}

			}
			
			sprite->_isStateChanged = false;
		}
	}

	// 特殊パーツのアップデート
	for (int partIndex = 0; partIndex < _currentAnimeRef->m_numParts; partIndex++)
	{
		CustomSprite* sprite = static_cast<CustomSprite*>(_parts.at(partIndex));

		//インスタンスパーツのアップデート
		if (sprite->_ssplayer)
		{
			sprite->_ssplayer->update(dt);
		}
		//エフェクトのアップデート
		if (sprite->refEffect)
		{
			sprite->refEffect->setParentSprite(sprite);

			//エフェクトアトリビュート
			int curKeyframe = sprite->_state.effectValue_curKeyframe;
			int refStartframe = sprite->_state.effectValue_startTime;
			float refSpeed = sprite->_state.effectValue_speed;
			bool independent = false;

			int lflags = sprite->_state.effectValue_loopflag;
			if (lflags & EFFECT_LOOP_FLAG_INDEPENDENT)
			{
				independent = true;
			}

			if (sprite->effectAttrInitialized == false)
			{
				sprite->effectAttrInitialized = true;
				sprite->effectTimeTotal = refStartframe;
			}

			sprite->refEffect->setParentSprite(sprite);	//親スプライトの設定
			if (sprite->_state.isVisibled == true)
			{

				if (independent)
				{
					//独立動作
					if (sprite->effectAttrInitialized)
					{
						float delta = dt / (1.0f / _animefps);						//	独立動作時は親アニメのfpsを使用する
						sprite->effectTimeTotal += delta * refSpeed;
						sprite->refEffect->setLoop(true);
						sprite->refEffect->setFrame(sprite->effectTimeTotal);
						sprite->refEffect->play();
						sprite->refEffect->update();
					}
				}
				else 
				{
					{
						float _time = frameNo - curKeyframe;
						if (_time < 0)
						{
						}
						else
						{
							_time *= refSpeed;
							_time = _time + refStartframe;
							sprite->effectTimeTotal = _time;

							sprite->refEffect->setSeedOffset(_seedOffset);
							sprite->refEffect->setFrame(_time);
							sprite->refEffect->play();
							sprite->refEffect->update();
						}
					}
				}
			}
		}
	}
	_prevDrawFrameNo = frameNo;	//再生したフレームを保存
}

//プレイヤーの描画
void Player::draw()
{
	_draw_count = 0;

	if (!_currentAnimeRef) return;

	ToPointer ptr(_currentRs->m_data);

	for (int index = 0; index < _currentAnimeRef->m_numParts; index++)
	{
		int partIndex = _partIndex[index];
		//スプライトの表示
		CustomSprite* sprite = static_cast<CustomSprite*>(_parts.at(partIndex));
		if (sprite->_ssplayer)
		{
			if ((sprite->_state.isVisibled == true) && (sprite->_state.opacity > 0))
			{
				//インスタンスパーツの場合は子供のプレイヤーを再生
				sprite->_ssplayer->draw();
				_draw_count += sprite->_ssplayer->getDrawSpriteCount();
			}
		}
		else
		{
			if (sprite->refEffect)
			{ 
				if ((sprite->_state.isVisibled == true) && (sprite->_state.opacity > 0))
				{
					//エフェクトパーツ
					sprite->refEffect->draw(m_textures);
					_draw_count = sprite->refEffect->getDrawSpriteCount();
				}
			}
			else
			{
				if (sprite->_state.texture.handle != -1)
				{
					if ((sprite->_state.isVisibled == true) && (sprite->_state.opacity > 0))
					{
						SSDrawSprite(sprite->_state);
						_draw_count++;
					}
				}
			}
		}
	}
}

void Player::checkUserData(int frameNo)
{
	ToPointer ptr(_currentRs->m_data);

	const AnimationData* animeData = _currentAnimeRef->m_animationData;

	if (!animeData->userData) return;
	const ss_offset* userDataIndex = static_cast<const ss_offset*>(ptr(animeData->userData));

	if (!userDataIndex[frameNo]) return;
	const ss_u16* userDataArray = static_cast<const ss_u16*>(ptr(userDataIndex[frameNo]));
	
	DataArrayReader reader(userDataArray);
	int numUserData = reader.readU16();

	for (int i = 0; i < numUserData; i++)
	{
		int flags = reader.readU16();
		int partIndex = reader.readU16();

		_userData.flags = 0;

		if (flags & UserData::FLAG_INTEGER)
		{
			_userData.flags |= UserData::FLAG_INTEGER;
			_userData.integer = reader.readS32();
		}
		else
		{
			_userData.integer = 0;
		}
		
		if (flags & UserData::FLAG_RECT)
		{
			_userData.flags |= UserData::FLAG_RECT;
			_userData.rect[0] = reader.readS32();
			_userData.rect[1] = reader.readS32();
			_userData.rect[2] = reader.readS32();
			_userData.rect[3] = reader.readS32();
		}
		else
		{
			_userData.rect[0] =
			_userData.rect[1] =
			_userData.rect[2] =
			_userData.rect[3] = 0;
		}
		
		if (flags & UserData::FLAG_POINT)
		{
			_userData.flags |= UserData::FLAG_POINT;
			_userData.point[0] = reader.readS32();
			_userData.point[1] = reader.readS32();
		}
		else
		{
			_userData.point[0] =
			_userData.point[1] = 0;
		}
		
		if (flags & UserData::FLAG_STRING)
		{
			_userData.flags |= UserData::FLAG_STRING;
			int size = reader.readU16();
			ss_offset offset = reader.readOffset();
			const char* str = ptr.toString(offset);
			_userData.str = str;
			_userData.strSize = size;
		}
		else
		{
			_userData.str = 0;
			_userData.strSize = 0;
		}
		
		const PartData* partData = _currentAnimeRef->getPartData(partIndex);
		_userData.partName = ptr.toString(partData->name);
		_userData.frameNo = frameNo;
		
		SSonUserData(this, &_userData);
	}

}

void  Player::setPosition(float x, float y)
{
	_state.x = x;
	_state.y = y;
}
void  Player::setRotation(float x, float y, float z)
{
	_state.rotationX = x;
	_state.rotationY = y;
	_state.rotationZ = z;
}

void  Player::setScale(float x, float y)
{
	_state.scaleX = x;
	_state.scaleY = y;
}

void  Player::setAlpha(int a)
{
	_state.opacity = a;
}

void  Player::setFlip(bool flipX, bool flipY)
{
	_state.flipX = flipX;
	_state.flipY = flipY;
}

//割合に応じた中間値を取得します
float Player::parcentVal(float val1, float val2, float parcent)
{
	float sa = val1 - val2;
	float newval = val2 + (sa * parcent);
	return (newval);
}
float Player::parcentValRot(float val1, float val2, float parcent)
{
	int ival1 = (int)(val1 * 10.0f) % 3600;
	int ival2 = (int)(val2 * 10.0f) % 3600;
	if (ival1 < 0)
	{
		ival1 += 3600;
	}
	if (ival2 < 0)
	{
		ival2 += 3600;
	}
	int islr = ival1 - ival2;
	if (islr < 0)
	{
		islr += 3600;
	}
	int inewval;
	if (islr == 0)
	{
		inewval = ival1;
	}
	else
	{
		if (islr > 1800)
		{
			int isa = 3600 - islr;
			inewval = ival2 - ((float)isa * parcent);
		}
		else
		{
			int isa = islr;
			inewval = ival2 + ((float)isa * parcent);
		}
	}


	float newval = (float)inewval / 10.0f;
	return (newval);
}

void Player::update_matrix_ss4(CustomSprite *sprite, CustomSprite *parent, const PartData *partData)
{


	if (partData->index == 0)
	{
		//rootパーツ
		//rootパーツはプレイヤーからステータスを引き継ぐ
		sprite->_state.x += _state.x;
		sprite->_state.y += _state.y;
		sprite->_state.rotationX += _state.rotationX;
		sprite->_state.rotationY += _state.rotationY;
		sprite->_state.rotationZ += _state.rotationZ;
		sprite->_state.scaleX *= _state.scaleX;
		sprite->_state.scaleY *= _state.scaleY;
		if (_state.flipX == true)
		{
			//プレイヤーのXフリップ
			sprite->_state.scaleX = -sprite->_state.scaleX;	//フラグ反転
		}
		if (_state.flipY == true)
		{
			sprite->_state.scaleY = -sprite->_state.scaleY;	//フラグ反転
		}

		sprite->_temp_position = SsVector3(sprite->_state.x, sprite->_state.y, 0);
		sprite->_temp_rotation = SsVector3(0, 0, sprite->_state.rotationZ);
		sprite->_temp_scale = SsVector2(sprite->_state.scaleX, sprite->_state.scaleY);

		return;
	}

	float x = sprite->_state.x;
	float y = sprite->_state.y;
	float z = sprite->_state.z;

	//scaleを適用
	x *= parent->_temp_scale.x;
	y *= parent->_temp_scale.y;

	float temp = 1.0f;

	//回転
	float angle = parent->_temp_rotation.z;
	double aa = DegreeToRadian(angle) * temp;
	double  asin = std::sin(aa);
	double  acos = std::cos(aa);

	float rx = (float)(x * acos - y * asin);
	float ry = (float)(x * asin + y * acos);

	x = rx;
	y = ry;

	//平行移動
	sprite->_temp_position.x = parent->_temp_position.x + x;
	sprite->_temp_position.y = parent->_temp_position.y + y;

	//ローカル回転
	sprite->_temp_rotation.z = sprite->_state.rotationZ + parent->_temp_rotation.z;

	//拡大率
	sprite->_temp_scale.x = sprite->_state.scaleX * parent->_temp_scale.x;
	sprite->_temp_scale.y = sprite->_state.scaleY * parent->_temp_scale.y;

	//SS4の状態で上書きする
//	sprite->_temp_position.x;
//	sprite->_temp_position.y;
//	sprite->_temp_rotation.z;
//	sprite->_temp_scale.x;
//	sprite->_temp_scale.y;
	
	//単位行列にする
	float mat[16];
	IdentityMatrix(mat);
	TranslationMatrixM(mat, sprite->_temp_position.x, sprite->_temp_position.y, sprite->_temp_position.z);//
	RotationXYZMatrixM(mat, DegreeToRadian(0), DegreeToRadian(0), DegreeToRadian(sprite->_temp_rotation.z * temp));
	ScaleMatrixM(mat, sprite->_temp_scale.x, sprite->_temp_scale.y, 1.0f);

	memcpy(sprite->_mat, mat, sizeof(float) * 16);
	memcpy(sprite->_state.mat, mat, sizeof(float) * 16);
	sprite->_state.Calc_rotationX = 0;
	sprite->_state.Calc_rotationY = 0;
	sprite->_state.Calc_rotationZ = sprite->_temp_rotation.z * temp;
	sprite->_state.Calc_scaleX = sprite->_temp_scale.x;
	sprite->_state.Calc_scaleY = sprite->_temp_scale.y;
}



};
