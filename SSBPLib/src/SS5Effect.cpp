#include "SS5Effect.h"
#include "SS5EventListener.h"
#include "effect/ssplayer_effect2.h"
#include "player/CustomSprite.h"
#include "player/ResourceSet.h"
#include "player/EffectCache.h"

namespace ss{


SsEffectRenderV2::SsEffectRenderV2(const ResourceSet* resource, SS5EventListener* eventListener, const std::string& effectName, int seed)
	: m_eventListener(eventListener)
	, m_resource(resource)
	, m_effectData(nullptr)
	, m_mySeed(seed * SEED_MAGIC)
	, m_nowFrame(0)
	, m_targetFrame(0)
	, m_effectTimeLength(0)
	, m_infinite(false)
	, m_isPlay(false)
	, m_isLoop(false)
	, m_seedOffset(0)
	, m_isWarningData(false)
	, m_parentSprite(nullptr)
	, m_drawSpritecount(0)
{
	SS_ASSERT(m_eventListener);
	SS_ASSERT(m_resource);

	//ロードイベントを投げてcellMapのテクスチャを取得する
	int cellMapNum = m_resource->m_cellCache->getCellMapNum();
	m_textures.resize(cellMapNum);
	for(int i = 0; i < cellMapNum; ++i){
		std::string textureName = m_resource->m_cellCache->getTexturePath(i);
		SsTexWrapMode wrapmode = m_resource->m_cellCache->getWrapMode(i);
		SsTexFilterMode filtermode = m_resource->m_cellCache->getFilterMode(i);

		m_textures[i] = m_eventListener->SSTextureLoad(textureName.c_str(), wrapmode, filtermode);
	}

	//利用するエフェクトデータをセットして初期化する
	m_effectData = m_resource->m_effectCache->getReference(effectName);
	SS_ASSERT(m_effectData);
	
	initialize();
}

SsEffectRenderV2::~SsEffectRenderV2()
{
	for(SsEffectEmitter* emitter : m_emmiterList){
		delete emitter;
	}

	m_emmiterList.clear();
	m_updateList.clear();

	//テクスチャの解放イベントを投げる
	for(TextureID textureID : m_textures){
		m_eventListener->SSTextureRelease(textureID);
	}
	m_textures.clear();
}



void SsEffectRenderV2::drawSprite(
	const CellRef* refCell,
	SsRenderBlendType blendType,
	const Matrix& localMatrix,
	SSColor4B	color,
	TextureID textureId
){
	Matrix matrix;

	if (m_parentSprite){
		matrix = m_parentSprite->_state.mat;
    	float parentAlphaRate = m_parentSprite->_state.opacity / 255.0f;
		color.a *= parentAlphaRate;
	}
	matrix = localMatrix * matrix;



	//セルの矩形から基本となる頂点座標を計算
	float width_h = refCell->m_rect.width() / 2;
	float height_h = refCell->m_rect.height() / 2;

	SSV3F_C4B_T2F_Quad quad;
	quad.tl.vertices = Vector3(-width_h,  height_h, 0);
	quad.tr.vertices = Vector3( width_h,  height_h, 0);
	quad.bl.vertices = Vector3(-width_h, -height_h, 0);
	quad.br.vertices = Vector3( width_h, -height_h, 0);
	//原点補正
	Vector3 center(
		(refCell->m_rect.width() * -(refCell->m_pivot.x)),
		(refCell->m_rect.height() * +(refCell->m_pivot.y)),	//todo:符号あってる？
		0.0
	);
	quad.vertexForeach([&](Vector3& vertex){
		vertex += center;		//原点補正
	});


	//UVを設定する
	quad.tl.texCoords = SSTex2F(refCell->m_uv1.u(), refCell->m_uv1.v());
	quad.tr.texCoords = SSTex2F(refCell->m_uv2.u(), refCell->m_uv1.v());
	quad.bl.texCoords = SSTex2F(refCell->m_uv1.u(), refCell->m_uv2.v());
	quad.br.texCoords = SSTex2F(refCell->m_uv2.u(), refCell->m_uv2.v());

	//カラー値を設定
	quad.tl.colors = color;
	quad.tr.colors = color;
	quad.bl.colors = color;
	quad.br.colors = color;

	//SSDrawSpriteから出しました-----------------------------------------------

	//vertexにworldMatrixをかける
	quad.vertexForeach([&](Vector3& vertex){
		vertex *= matrix;
	});

	//頂点カラーにアルファを設定
	int calc_opacity = m_parentSprite->_state.Calc_opacity;
	quad.tl.colors.a = quad.bl.colors.a * calc_opacity / 255;
	quad.tr.colors.a = quad.bl.colors.a * calc_opacity / 255;
	quad.bl.colors.a = quad.bl.colors.a * calc_opacity / 255;
	quad.br.colors.a = quad.bl.colors.a * calc_opacity / 255;


	
	//ブレンドタイプを設定
	BlendType blendfunc = BLEND_ADD;
	if(blendType == SsRenderBlendType::Mix){
		blendfunc = BLEND_MIX;	//ブレンドタイプを設定
	}
	BlendType colorBlendVertexFunc = BLEND_MUL;		//カラーブレンドフラグ乗算
	int colorBlendVertexFlags = VERTEX_FLAG_ONE;	//カラーブレンドフラグを設定 //memo:意味合いから考えてこれで合ってるはず(todo:Color機能ONのときだけの設定にする必要はあるかも)。色味が変(そもそも元から変だが)なときはここを疑う

	m_eventListener->SSDrawSprite(quad, textureId, blendfunc, colorBlendVertexFunc, colorBlendVertexFlags);	//描画

	m_drawSpritecount++;
}


void SsEffectRenderV2::particleDraw(SsEffectEmitter* e , double time , SsEffectEmitter* parent , particleDrawData* plp)
{
	double t = time;

	if (e == 0) return;

	int pnum = e->getParticleIDMax();

	int slide = (parent == 0) ? 0 : plp->id;
	e->updateEmitter(time, slide);

	for (auto id = 0; id < pnum; id++)
	{
		const particleExistSt* drawe = e->getParticleDataFromID(id);

        if ( !drawe->born )continue;

		float targettime = (t + 0.0f);
		particleDrawData lp;
		particleDrawData pp;
		pp.x = 0; pp.y = 0;

		lp.id = id + drawe->cycle;
		lp.stime = drawe->stime;
		lp.lifetime = drawe->endtime;
		lp.pid = 0;

		if ( parent )lp.pid = plp->id;

		//if ( lp.stime == lp.lifetime ) continue;

		//if ( lp.stime <= targettime && lp.lifetime >= targettime)
		if ( drawe->exist )
		{

			if (parent)
			{
				//親から描画するパーティクルの初期位置を調べる
				pp.id = plp->id;
				pp.stime = plp->stime;
				pp.lifetime = plp->lifetime;
				pp.pid = plp->pid;
				//パーティクルが発生した時間の親の位置を取る

				int ptime = lp.stime + pp.stime;
				if ( ptime > lp.lifetime ) ptime = lp.lifetime;

				//逆算はデバッグしずらいかもしれない
				parent->updateParticle( lp.stime + pp.stime , &pp);
				e->position.x = pp.x;
				e->position.y = pp.y;

			}


			e->updateParticle(targettime, &lp);

			if(e->refData->getCellRef()){
				Matrix localTransformMatrix = lp.craeteLocalTransformMatrix(m_effectData->layoutScale());
				drawSprite(
					e->refData->getCellRef(), e->refData->getBlendType(),
					localTransformMatrix, lp.color, m_textures[e->refData->getCellRef()->m_cellMapIndex]
				);
			}
		}

	}



}



//パラメータをコピーする
void SsEffectRenderV2::initEmitter( SsEffectEmitter* e , const SsEffectNode* node)
{

	e->refData = node;
	for(const SsEffectElementBase* element : e->refData->plist){
		element->initalizeEffect(e);
	}

	e->emitterSeed = m_mySeed;

	if ( e->particle.userOverrideRSeed ){
		e->emitterSeed = e->particle.overrideRSeed;
	}
	else{
		if ( m_effectData->isLockRandSeed() ){
			e->emitterSeed = (m_effectData->lockRandSeed()+1) * SEED_MAGIC;
		}
	}

	e->emitter.life+= e->particle.delay;//ディレイ分加算
}




void SsEffectRenderV2::update()
{

	if ( !m_isPlay ) return;

	m_targetFrame = m_nowFrame;

	if ( !m_infinite )
	{
		if (this->isloop()) //自動ループの場合
		{
			if (m_nowFrame > getEffectTimeLength())
			{
				m_targetFrame = (int)((int)m_nowFrame % getEffectTimeLength());
				int l = (m_nowFrame / getEffectTimeLength());
				setSeedOffset(l);
			}
		}
	}
}

void SsEffectRenderV2::draw()
{
	m_drawSpritecount = 0;	//表示スプライト数のクリア

	if (m_nowFrame < 0) return;

	for (SsEffectEmitter* e : m_updateList){
		if (e){
			e->setSeedOffset(m_seedOffset);
		}
	}

	for (SsEffectEmitter* e : m_updateList){

		if ( e->_parent )
		{
			//グローバルの時間で現在親がどれだけ生成されているのかをチェックする
			e->_parent->updateEmitter(m_targetFrame, 0);

			int loopnum =  e->_parent->getParticleIDMax();
			for ( int n = 0 ; n < loopnum ; n ++ )
			{
				const particleExistSt* drawe = e->_parent->getParticleDataFromID(n);

				if ( drawe->born )
				{
					particleDrawData lp;
					lp.stime = drawe->stime;
					lp.lifetime = drawe->endtime;
					lp.id = n;
					lp.pid = 0;

					float targettime = (m_targetFrame + 0.0f);
					float ptime = (targettime - lp.stime );

	  				particleDraw( e , ptime , e->_parent , &lp);
				}
			}

		}else{
			particleDraw( e , m_targetFrame ,nullptr, nullptr);
		}
	}

}


bool compare_priority( SsEffectEmitter* left,  SsEffectEmitter* right)
{
	if (left->priority == right->priority)
	{
		if (left->uid < right->uid) return true;
	}
	//	return true;
  return left->priority < right->priority ;
}



void SsEffectRenderV2::initialize()
{
	const std::vector<const SsEffectNode*>& list = m_effectData->getNodeList();

	std::vector<int> cnum(list.size(), 0);

	//パラメータを取得
	//以前のデータ形式から変換
	for (size_t i = 0; i < list.size(); i++)
	{
		const SsEffectNode *node = list[i];

		if (node->GetType() == SsEffectNodeType::emmiter){
			SsEffectEmitter* e = new SsEffectEmitter();
			//パラメータをコピー

			e->_parentIndex = node->getParentIndex();
			//繋ぎ先は恐らくパーティクルなのでエミッタに変換
			if (e->_parentIndex != 0){
				e->_parentIndex = list[e->_parentIndex]->getParentIndex();
			}

			cnum[e->_parentIndex]++;
			if (cnum[e->_parentIndex] > 10){
				m_isWarningData = true;
				delete e;
				continue; //子１０ノード表示制限
			}

			//孫抑制対策
			if (e->_parentIndex != 0){
				int a = list[e->_parentIndex]->getParentIndex();
				if (a != 0){
					if (list[a]->getParentIndex() > 0) {
						m_isWarningData = true;
						delete e;
						continue;
					}
				}
			}

			initEmitter(e, node);
			m_emmiterList.push_back(e);
			if(e->emitter.Infinite){
				m_infinite = true;
			}
		}
		else{
			//エミッター同士を繋ぎたいので
			m_emmiterList.push_back(nullptr);
		}
	}

	//親子関係整理


	m_effectTimeLength = 0;
	//事前計算計算  updateListにルートの子を配置し親子関係を結ぶ
	for (size_t i = 0; i < m_emmiterList.size(); i++)
	{

		if (m_emmiterList[i] != 0)
		{
			m_emmiterList[i]->uid = i;
			m_emmiterList[i]->precalculate2(); //ループ対応形式


			int  pi = m_emmiterList[i]->_parentIndex;

			if (m_emmiterList[i]->_parentIndex == 0)  //ルート直下
			{
				m_emmiterList[i]->_parent = 0;
				m_emmiterList[i]->globaltime = m_emmiterList[i]->getTimeLength();
				m_updateList.push_back(m_emmiterList[i]);
			}
			else
			{

				void* t = m_emmiterList[pi];

				m_emmiterList[i]->_parent = m_emmiterList[pi];

				m_emmiterList[i]->globaltime = m_emmiterList[i]->getTimeLength() + m_emmiterList[pi]->getTimeLength();

				m_updateList.push_back(m_emmiterList[i]);
			}

			if (m_emmiterList[i]->globaltime > m_effectTimeLength)
			{
				m_effectTimeLength = m_emmiterList[i]->globaltime;
			}
		}
	}
	//プライオリティソート
	std::sort(m_updateList.begin(), m_updateList.end(), compare_priority);
}


size_t SsEffectRenderV2::getEffectTimeLength()
{

	return m_effectTimeLength;
}

#if 0
int	SsEffectRenderV2::getCurrentFPS(){
	if (m_effectData)
	{
		if (m_effectData->fps == 0 ) return 30;

		return m_effectData->fps;
	}
	return 30;
}
#endif

void SsEffectRenderV2::setSeedOffset(int offset)
{
	if(m_effectData->isLockRandSeed()){
		m_seedOffset = 0;
	}
	else{
		m_seedOffset = offset;
	}	
}


} //namespace ss
