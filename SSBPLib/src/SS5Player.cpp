// 
//  SS5Player.cpp
//
#include "SS5Player.h"
#include "SS5PlayerData.h"
#include "SS5PlayerTypes.h"
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
#include "player/InstancePartStatus.h"
#include "ResluteState.h"
#include "ResourceManager.h"
#include "SS5EventListener.h"
#include "effect/ssplayer_effect2.h"


namespace ss{


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

Player::Player(const ResourceSet* resource, SS5EventListener* eventListener)
	: _eventListener(eventListener)
	, _currentRs(resource)
	, _currentAnimeRef(nullptr)
	, _currentFrameTime(0.0f)
	, _isPausing(false)
	, _motionBlendPlayer(nullptr)
	, _blendTime(0.0f)
	, _blendTimeMax(0.0f)
	, _seedOffset(0)
{
	SS_ASSERT_LOG(_eventListener, "eventListener is null");
	SS_ASSERT_LOG(_currentRs, "resource is null");

	for (int i = 0; i < PART_VISIBLE_MAX; i++){
		_partVisible[i] = true;
		_partIndex[i] = -1;
		_cellChange[i] = -1;
	}

	//ロードイベントを投げてcellMapのテクスチャを取得する
	int cellMapNum = _currentRs->m_cellCache->getCellMapNum();
	m_textures.resize(cellMapNum);
	for(int i = 0; i < cellMapNum; ++i){
		std::string textureName = _currentRs->m_cellCache->getTexturePath(i);	
		SsTexWrapMode wrapmode = _currentRs->m_cellCache->getWrapMode(i);
		SsTexFilterMode filtermode = _currentRs->m_cellCache->getFilterMode(i);

		TextuerData& texdata = m_textures[i];
		texdata.handle = _eventListener->SSTextureLoad(textureName.c_str(), wrapmode, filtermode); // wrapmode, filtermode);//todo:事前にテクスチャ情報取得できるようにする
		_eventListener->SSGetTextureSize(texdata.handle, &(texdata.size_w), &(texdata.size_h));
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
		_eventListener->SSTextureRelease(texdata.handle);
	}
	m_textures.clear();
}


int Player::getMaxFrame() const
{
	if (_currentAnimeRef ){	//todo:playerが作られた時点で、_currentAnimeRefがセットされていることにしたい(ifがなくなる)
		return(_currentAnimeRef->m_animationData->numFrames);
	}
	return 0;
}

int Player::getCurrentFrame() const
{
	return static_cast<int>(_currentFrameTime);
}

void Player::setCurrentFrame(int frame)
{
	_currentFrameTime = frame;
}


void Player::play(const std::string& ssaeName, const std::string& motionName, int startFrameNo)
{
	std::string animeName = ssaeName + "/" + motionName;
	play(animeName, startFrameNo);
}

void Player::play(const std::string& animeName, int startFrameNo)
{
	SS_ASSERT_LOG(_currentRs != NULL, "Not select data");

	AnimeRef* animeRef = _currentRs->m_animeCache->getReference(animeName);
	SS_ASSERT_LOG(animeRef, "Not found animation > anime=%s", animeName.c_str());
	
	_currentAnimename = animeName;

	play(animeRef, startFrameNo);
}

void Player::play(AnimeRef* animeRef, int startFrameNo)
{
	if (_currentAnimeRef != animeRef)
	{
		_currentAnimeRef = animeRef;
		
		allocParts(animeRef->m_numParts, false);
		setPartsParentage();
	}
	_currentFrameTime = startFrameNo;
	_isPausing = false;
	
	setFrame(static_cast<int>(_currentFrameTime));

	//play実行時に最初のフレームのユーザーデータを確認する
	checkUserData(static_cast<int>(_currentFrameTime));
}

//モーションブレンドしつつ再生
void Player::motionBlendPlay(const std::string& animeName, /*int loop,*/ int startFrameNo, float blendTime)
{
	if (_currentAnimename != "")
	{
		//現在のアニメーションをブレンド用プレイヤーで再生
		if (_motionBlendPlayer == NULL)
		{
			_motionBlendPlayer = new Player(_currentRs, _eventListener);	//とりあえず今のイベントリスナーを突っ込んでおく	//todo:後で整理する
		}
	#if 0
		int loopnum = _loop;
		if (_loop > 0)
		{
			loopnum = _loop - _loopCount;
		}
		_motionBlendPlayer->play(_currentAnimename, loopnum, getFrameNo());	//今までのアニメーションの状態(ループ含む)を持ち越すらしいが、start,endとかの伝播ができてないようだ
		if (_loop > 0)
		{
			if (_loop == _loopCount)	//アニメは最後まで終了している
			{
				_motionBlendPlayer->stop();
			}
		}
	#else
		//todo:モーションブレンドでのループは別途考える
		_motionBlendPlayer->play(_currentAnimename, getCurrentFrame());
	#endif
		_blendTime = 0;
		_blendTimeMax = blendTime;

	}
	play(animeName, startFrameNo);

}



void Player::resume()
{
	_isPausing = false;
}

void Player::stop()
{
	_isPausing = true;
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
	if (!_currentAnimeRef) return;
	if (!_currentRs->m_data) return;

	if(_isPausing){
		//アニメを手動で更新する場合
		checkUserData(getCurrentFrame());
	}
	else{
		// フレームを進める.
		float nextFrameTime = _currentFrameTime + (dt * getAnimeFPS());
		float nextFrameRemainder = nextFrameTime - static_cast<int>(nextFrameTime);
		
		int checkFrame = getCurrentFrame();
		int seekCount = nextFrameTime - getCurrentFrame();
		// 順再生時.
		for(int i = 0; i < seekCount; ++i){
			checkFrame = _eventListener->limitFrame(this, checkFrame + 1, getMaxFrame());	//範囲制限
			SS_ASSERT_LOG(0 <= checkFrame && checkFrame < getMaxFrame(), "checkFrame is out of range. checkFrame=%d", checkFrame);
			
			// このフレームのユーザーデータをチェック
			checkUserData(checkFrame);

			if(checkFrame == 0){	//一巡した
				_seedOffset++;	//シードオフセットを加算
			}
		}
		// 逆再生時.
		for(int i = 0; i > seekCount; --i){
			checkFrame = _eventListener->limitFrame(this, checkFrame - 1, getMaxFrame());	//範囲制限
			SS_ASSERT_LOG(0 <= checkFrame && checkFrame < getMaxFrame(), "checkFrame is out of range. checkFrame=%d", checkFrame);

			// このフレームのユーザーデータをチェック
			checkUserData(checkFrame);

			if(checkFrame == getMaxFrame()-1){	//一巡した
				_seedOffset++;	//シードオフセットを加算
			}
		}
		
		_currentFrameTime = checkFrame + nextFrameRemainder;
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

	setFrame(getCurrentFrame(), dt);
}


void Player::allocParts(int numParts, bool useCustomShaderProgram)
{
	releaseParts();	//すべてのパーツを消す

	// パーツ数だけCustomSpriteを作成する
	for (int i = 0; i < numParts; i++){
		CustomSprite* sprite =  new CustomSprite();		
		_parts.push_back(sprite);
	}
}

void Player::releaseParts()
{
	SS_ASSERT(_currentRs);
	SS_ASSERT(_currentAnimeRef);

	// パーツの子CustomSpriteを全て削除
	for(int i = 0; i < _parts.size(); ++i){
		CustomSprite* sprite = _parts[i];
	
		//ChildPlayerがあるなら、spriteを破棄する前にリリースイベントを飛ばす
		if(sprite->_haveChildPlayer){	//todo:customspriteでやる?
			_eventListener->ChildPlayerRelease(i, getPartName(i));
		}
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
		CustomSprite* sprite = _parts.at(partIndex);
		
		if (partIndex > 0){
			sprite->_parent = _parts.at(partData->parentIndex);
		}
		else{
			sprite->_parent = nullptr;
		}

		//インスタンスパーツならChildPlayerの生成イベントを飛ばす
		if(partData->type == PARTTYPE_INSTANCE){
			std::string refanimeName = ptr.toString(partData->refname);
			sprite->_haveChildPlayer = _eventListener->ChildPlayerLoad(partIndex, getPartName(partIndex), refanimeName);
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
			const SsEffectModel* effectmodel = _currentRs->m_effectCache->getReference(refeffectName);
			if (effectmodel)
			{
				//エフェクトクラスにパラメータを設定する
				sprite->refEffect = new SsEffectRenderV2(effectmodel, getRandomSeed());
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

	const PartData* partData = _currentAnimeRef->getPartData(partId);
	const char* name = ptr.toString(partData->name);
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
		
		//カレントフレームのパーツステータスを取得する
		if (frameNo == -1)
		{
			//フレームの指定が省略された場合は現在のフレームを使用する
			frameNo = getCurrentFrame();
		}

		if (frameNo != getCurrentFrame())
		{
			//取得する再生フレームのデータが違う場合プレイヤーを更新する
			//パーツステータスの更新
			setFrame(frameNo);
		}

		ToPointer ptr(_currentRs->m_data);

		int partIndex = indexOfPart(name);
		if(partIndex != -1){

			const PartData* partData = _currentAnimeRef->getPartData(partIndex);

			//必要に応じて取得するパラメータを追加してください。
			//当たり判定などのパーツに付属するフラグを取得する場合は　partData　のメンバを参照してください。
			//親から継承したスケールを反映させる場合はxスケールは_mat.m[0]、yスケールは_mat.m[5]をかけて使用してください。
			CustomSprite* sprite = _parts.at(partIndex);
			//パーツアトリビュート
	//					sprite->_state;												//SpriteStudio上のアトリビュートの値は_stateから取得してください
			result.flags = sprite->_state.flags;						// このフレームで更新が行われるステータスのフラグ
			result.cellIndex = sprite->_state.cellIndex;				// パーツに割り当てられたセルの番号
			sprite->_state.mat.getTranslation(&result.x, &result.y);
			result.z = sprite->_state.z;				//todo:意味合いとしてはgetTranslationで取得すればいいはず
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
			if(colorName == COLORLABELSTR_NONE){
				result.part_labelcolor = COLORLABEL_NONE;
			}
			if(colorName == COLORLABELSTR_RED){
				result.part_labelcolor = COLORLABEL_RED;
			}
			if(colorName == COLORLABELSTR_ORANGE){
				result.part_labelcolor = COLORLABEL_ORANGE;
			}
			if(colorName == COLORLABELSTR_YELLOW){
				result.part_labelcolor = COLORLABEL_YELLOW;
			}
			if(colorName == COLORLABELSTR_GREEN){
				result.part_labelcolor = COLORLABEL_GREEN;
			}
			if(colorName == COLORLABELSTR_BLUE){
				result.part_labelcolor = COLORLABEL_BLUE;
			}
			if(colorName == COLORLABELSTR_VIOLET){
				result.part_labelcolor = COLORLABEL_VIOLET;
			}
			if(colorName == COLORLABELSTR_GRAY){
				result.part_labelcolor = COLORLABEL_GRAY;
			}

			rc = true;
		}
			
		//パーツステータスを表示するフレームの内容で更新
		if (frameNo != getCurrentFrame())
		{
			//取得する再生フレームのデータが違う場合プレイヤーの状態をもとに戻す
			//パーツステータスの更新
			setFrame(getCurrentFrame());
		}
		
	}
	return rc;
}


//ラベル名からラベルの設定されているフレームを取得
//ラベルが存在しない場合は戻り値が-1となります。
//ラベル名が全角でついていると取得に失敗します。
int Player::getLabelToFrame(char* findLabelName)
{
	ToPointer ptr(_currentRs->m_data);
	const AnimationData* animeData = _currentAnimeRef->m_animationData;

	if (!animeData->labelData) return -1;
	const ss_offset* labelDataIndex = static_cast<const ss_offset*>(ptr(animeData->labelData));


	for (int idx = 0; idx < animeData->labelNum; idx++ ){

		if (!labelDataIndex[idx]) return -1;
		const ss_u16* labelDataArray = static_cast<const ss_u16*>(ptr(labelDataIndex[idx]));

		DataArrayReader reader(labelDataArray);

		LabelData ldata;
		ss_offset offset = reader.readOffset();
		const char* str = ptr.toString(offset);
		int labelFrame = reader.readU16();
		ldata.str = str;
		ldata.frameNo = labelFrame;

		if (ldata.str.compare(findLabelName) == 0 ){
			return ldata.frameNo;		//同じ名前のラベルが見つかった
		}
	}

	return -1;
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
	int changeCellIndex = -1;
	if ((sscename != "") && (cellname != "")){
		changeCellIndex = _currentRs->m_cellCache->indexOfCell(cellname, sscename);
	}

	int partIndex = indexOfPart(partsname.c_str());
	_cellChange[partIndex] = changeCellIndex;	//セル番号を設定
	//memo:元の実装では_partInex[]のインデックスを使っていたので動作がおかしいときはそのあたりを疑ってみる
}


//スプライト情報の取得
const CustomSprite* Player::getSpriteData(int partIndex) const
{
	if(_parts.size() < partIndex){
		return nullptr;		//todo:assertでいいような気がする
	}
	return _parts.at(partIndex);
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


		//セルの原点設定を反映させる
		const CellRef* cellRef = state.cellIndex >= 0 ? _currentRs->m_cellCache->getReference(state.cellIndex) : nullptr;
		if (cellRef){
			float cpx = cellRef->m_cell->pivot_X;
			float cpy = cellRef->m_cell->pivot_Y;

			if(state.flipX){ cpx = -cpx; }	// 水平フリップによって原点を入れ替える
			if(state.flipY){ cpy = -cpy; }	// 垂直フリップによって原点を入れ替える

			state.pivotX += cpx;
			state.pivotY += cpy;
		}
		state.pivotX += 0.5f;
		state.pivotY += 0.5f;

		//モーションブレンド
		if (_motionBlendPlayer)
		{
			const CustomSprite* blendSprite = _motionBlendPlayer->getSpriteData(partIndex);
			if (blendSprite)
			{ 
				float percent = _blendTime / _blendTimeMax;
				state.x = lerp(blendSprite->_orgState.x, state.x, percent);
				state.y = lerp(blendSprite->_orgState.y, state.y, percent);
				state.scaleX = lerp(blendSprite->_orgState.scaleX, state.scaleX, percent);
				state.scaleY = lerp(blendSprite->_orgState.scaleY, state.scaleY, percent);
				state.rotationX = lerpAngle(blendSprite->_orgState.rotationX, state.rotationX, percent);
				state.rotationY = lerpAngle(blendSprite->_orgState.rotationY, state.rotationY, percent);
				state.rotationZ = lerpAngle(blendSprite->_orgState.rotationZ, state.rotationZ, percent);
			}

		}

		CustomSprite* sprite = _parts.at(partIndex);

		if (cellRef){
			//各パーツのテクスチャ情報を設定
			state.texture = m_textures[cellRef->m_cellMapIndex]; //cellRef->m_texture;
			state.rect = cellRef->m_rect;
			state.blendfunc = static_cast<BlendType>(partData->alphaBlendType);
		}
		else{
			state.texture.handle = -1;
			//セルが無く通常パーツ、ヌルパーツの時は非表示にする
			if ((partData->type == PARTTYPE_NORMAL) || (partData->type == PARTTYPE_NULL)){
				state.isVisibled = false;
			}
		}
		
		//頂点データの設定
		//quadにはプリミティブの座標（頂点変形を含む）、UV、カラー値が設定されます。
		SSV3F_C4B_T2F_Quad quad;
		memset(&quad, 0, sizeof(quad));
		SSRect cellRect;
		if (cellRef){
			cellRect = cellRef->m_rect;
		}
		state.vertexCompute(&quad, cellRect);

		// 頂点変形のオフセット値を反映
		if (state.flags & PART_FLAG_VERTEX_TRANSFORM){
			SSQuad3 positionOffsets;
			positionOffsets.readVertexTransform(reader);
			
			quad.add(positionOffsets);
		}
		
		//頂点情報の取得
		SSColor4B color4 = { 0xff, 0xff, 0xff, 0xff };
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

			sprite->_state.colorBlendFunc = static_cast<BlendType>(funcNo);
			sprite->_state.colorBlendType = cb_flags;

			//ssbpではカラーブレンドのレート（％）は使用できません。
			//制限となります。
			if (cb_flags & VERTEX_FLAG_ONE){

				color4.readColorWithRate(reader);
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
		quad.colorsForeach([&](SSColor4B& color){
			color.r *= (_playerSetting.m_col_r / 255.0);
			color.g *= (_playerSetting.m_col_g / 255.0);
			color.b *= (_playerSetting.m_col_b / 255.0);
			color.a *= (state.opacity / 255.0);
		});


		//UVを設定する
		SSTex2F uv_tl, uv_br;
		if(cellRef){
			uv_tl = SSTex2F(cellRef->m_cell->u1, cellRef->m_cell->v1);
			uv_br = SSTex2F(cellRef->m_cell->u2, cellRef->m_cell->v2);
		}
		state.uvCompute(&quad, uv_tl, uv_br);

		state.quad = quad;


		//スプライトステータスの保存
		sprite->setState(state);
		sprite->_orgState = sprite->_state;

	}


	// 親に変更があるときは自分も更新するようフラグを設定する
	for (int partIndex = 1; partIndex < _currentAnimeRef->m_numParts; partIndex++)
	{
		const PartData* partData = _currentAnimeRef->getPartData(partIndex);
		CustomSprite* sprite = _parts.at(partIndex);
		CustomSprite* parent = _parts.at(partData->parentIndex);
		
		if (parent->_isStateChanged){
			sprite->_isStateChanged = true;
		}
	}

	// 行列の更新
	for (int partIndex = 0; partIndex < _currentAnimeRef->m_numParts; partIndex++)
	{
		const PartData* partData = _currentAnimeRef->getPartData(partIndex);
		CustomSprite* sprite = _parts.at(partIndex);

		if (sprite->_isStateChanged){
			Matrix mat;
			
			if (partIndex > 0){
				//親のマトリクスを適用
				CustomSprite* parent = _parts.at(partData->parentIndex);
				mat = parent->_mat;
			}
			else{				
				//rootパーツはプレイヤーからステータスを引き継ぐ
				_playerSetting.getTransformMatrix(&mat);
			}
			// SRzRyRxT mat
			Matrix localTransformMatrix;
			localTransformMatrix.setupSRzyxT(
				Vector3(sprite->_state.scaleX, sprite->_state.scaleY, 1.0f),
				Vector3(SSDegToRad(sprite->_state.rotationX), SSDegToRad(sprite->_state.rotationY), SSDegToRad(sprite->_state.rotationZ)),
				Vector3(sprite->_state.x, sprite->_state.y, 0.0f)
			);
			mat = localTransformMatrix * mat;

			sprite->_mat = mat;
			sprite->_state.mat = mat;

			if (partIndex > 0)
			{
				CustomSprite* parent = _parts.at(partData->parentIndex);
				//子供は親のステータスを引き継ぐ
				//ルートパーツのアルファ値を反映させる
				sprite->_state.Calc_opacity = (sprite->_state.Calc_opacity * _playerSetting.m_opacity) / 255;
				
				//インスタンスアニメーションがある場合は親パーツ情報を通知する
				if(sprite->_haveChildPlayer){
					Vector3 pos, rot, scale;
					sprite->_mat.getTranslation(&pos.x, &pos.y, &pos.z);
					sprite->_mat.getRotation(&rot.x, &rot.y, &rot.z);
					sprite->_mat.getScale(&scale.x, &scale.y, &scale.z);
					SSColor4B col(_playerSetting.m_col_r, _playerSetting.m_col_g, _playerSetting.m_col_b, sprite->_state.Calc_opacity);
					ParentPartState parentPartState = {
						pos, rot, scale, col
					};
					InstancePartStatus ips = sprite->_state.instanceValue;
					_eventListener->ChildPlayerSetFrame(		//todo:行列そのものを渡すようにすべき
						partIndex, getPartName(partIndex),
						parentPartState, ips.getFrame(frameNo), ips.m_independent
					);
				}
			}
			
			sprite->_isStateChanged = false;
		}
	}

	// 特殊パーツのアップデート
	for (int partIndex = 0; partIndex < _currentAnimeRef->m_numParts; partIndex++)
	{
		CustomSprite* sprite = _parts.at(partIndex);

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
			if (lflags & EFFECT_LOOP_FLAG_INDEPENDENT){
				independent = true;
			}

			if (sprite->effectAttrInitialized == false){
				sprite->effectAttrInitialized = true;
				sprite->effectTimeTotal = refStartframe;
			}

			sprite->refEffect->setParentSprite(sprite);	//親スプライトの設定
			if (sprite->_state.isVisibled == true){

				if (independent){
					//独立動作
					if (sprite->effectAttrInitialized){
						float delta = dt / (1.0f / getAnimeFPS());						//	独立動作時は親アニメのfpsを使用する
						sprite->effectTimeTotal += delta * refSpeed;
						sprite->refEffect->setLoop(true);
						sprite->refEffect->setFrame(sprite->effectTimeTotal);
						sprite->refEffect->play();
						sprite->refEffect->update();
					}
				}
				else {
					
					float time = frameNo - curKeyframe;
					if (time < 0){
					}
					else{
						time *= refSpeed;
						time = time + refStartframe;
						sprite->effectTimeTotal = time;

						sprite->refEffect->setSeedOffset(_seedOffset);
						sprite->refEffect->setFrame(time);
						sprite->refEffect->play();
						sprite->refEffect->update();
					}
				}
			}
		}
	}
}

//プレイヤーの描画
void Player::draw()
{
	_draw_count = 0;

	if (!_currentAnimeRef) return;

	for (int index = 0; index < _currentAnimeRef->m_numParts; index++)
	{
		int partIndex = _partIndex[index];
		//スプライトの表示
		CustomSprite* sprite = _parts.at(partIndex);
		const State& state = sprite->_state;
		if (sprite->_haveChildPlayer){
			if ((state.isVisibled == true) && (state.opacity > 0)){
				_eventListener->ChildPlayerDraw(partIndex, getPartName(partIndex));
			}
		}
		else{
			if (sprite->refEffect){ 
				if ((state.isVisibled == true) && (state.opacity > 0)){
					//エフェクトパーツ
					sprite->refEffect->draw(m_textures);
					_draw_count = sprite->refEffect->getDrawSpriteCount();
				}
			}
			else{
				if (state.texture.handle != -1){
					if ((state.isVisibled == true) && (state.opacity > 0)){
						SSDrawSprite(state, state.blendfunc, state.colorBlendFunc);
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

	for (int i = 0; i < numUserData; i++){
		UserData userData;
		userData.readData(reader, ptr);
		_eventListener->onUserData(this, userData, frameNo);
	}

}


int Player::getAnimeFPS() const{
	SS_ASSERT(_currentAnimeRef);
	return _currentAnimeRef->m_animationData->fps;
}

/** プレイヤーへの各種設定 ------------------------------*/
void Player::setPosition(float x, float y){
	_playerSetting.m_position = Vector3(x, y, 0.0f);
}
void Player::setRotation(float x, float y, float z){
	_playerSetting.m_rotation = Vector3(x, y, z);
}
void Player::setScale(float x, float y){
	_playerSetting.m_scale = Vector3(x, y, 1.0f);
}

void Player::setAlpha(int a){
	_playerSetting.m_opacity = a;
}

//アニメーションの色成分を変更します
void Player::setColor(int r, int g, int b)
{
	_playerSetting.m_col_r = r;
	_playerSetting.m_col_g = g;
	_playerSetting.m_col_b = b;
}
/*-------------------------------------------------------*/


};
