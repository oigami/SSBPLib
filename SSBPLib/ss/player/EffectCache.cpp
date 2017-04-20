#include "EffectCache.h"
#include <string>
#include <map>
#include "DataArrayReader.h"
#include "player/Util.h"
#include "player/ToPointer.h"
#include "player/CellCache.h"
#include "effect/ssloader_ssee.h"
using namespace std;

namespace ss{


EffectCache::EffectCache(const ProjectData* data, const CellCache* cellCache)
{
	init(data, cellCache);
}
EffectCache::~EffectCache()
{
	for(auto str_model : m_dic){
		SS_SAFE_DELETE(str_model.second);
	}
	m_dic.clear();
}


/**
* エフェクトファイル名を指定してEffectRefを得る
*/
const SsEffectModel* EffectCache::getReference(const string& name) const
{
	const SsEffectModel* ref = m_dic.at(name);
	return ref;
}


void EffectCache::init(const ProjectData* data, const CellCache* cellCache)
{
	SS_ASSERT_LOG(data != NULL, "Invalid data");

	ToPointer ptr(data);

	//ssbpからエフェクトファイル配列を取得
	const EffectFile* effectFileArray = ptr.toEffectFiles(data);

	for(int listindex = 0; listindex < data->numEffectFileList; listindex++)
	{
		//エフェクトファイル配列からエフェクトファイルを取得
		const EffectFile* effectFile = &effectFileArray[listindex];
		string effectFileName = ptr.toString(effectFile->name);

		//保持用のエフェクトファイル情報を作成
		SsEffectModel *effectmodel = new SsEffectModel(
			effectFileName, effectFile->fps,
			effectFile->isLockRandSeed, effectFile->lockRandSeed,
			static_cast<float>(effectFile->layoutScaleX) / 100.0f,
			static_cast<float>(effectFile->layoutScaleY) / 100.0f
		);


		//エフェクトファイルからエフェクトノード配列を取得
		const EffectNode* effectNodeArray = ptr.toEffectNodes(effectFile);
		for(int nodeindex = 0; nodeindex < effectFile->numNodeList; nodeindex++)
		{
			const EffectNode* effectNode = &effectNodeArray[nodeindex];		//エフェクトノード配列からエフェクトノードを取得
			
			int cellIndex = effectNode->cellIndex;
			const CellRef* cellRef = (cellIndex >= 0) ? cellCache->getReference(cellIndex) : nullptr;

			//セル情報を作成
			SsEffectNode *node = new SsEffectNode(
				effectNode->parentIndex,
				static_cast<SsEffectNodeType>(effectNode->type),
				static_cast<SsRenderBlendType>(effectNode->blendType),
				cellRef
			);

			//エフェクトノードからビヘイビア配列を取得
			const ss_offset* behaviorArray = static_cast<const ss_offset*>(ptr(effectNode->Behavior));
			for(int behaviorindex = 0; behaviorindex < effectNode->numBehavior; behaviorindex++)
			{
				//ビヘイビア配列からビヘイビアを取得
				const ss_u16* behavior_adr = static_cast<const ss_u16*>(ptr(behaviorArray[behaviorindex]));
				DataArrayReader reader(behavior_adr);

				//パラメータを作ってpush_backで登録していく
				int type = reader.readS32();
				SsEffectElementBase* effectParam = SsEffectElementBase::create( static_cast<SsEffectFunctionType>(type) );
				if(effectParam){
					effectParam->readData(reader);			//データ読み込み
					node->addElement(effectParam);
				}
			}


			effectmodel->addNode(node);
		}

		//SS_LOG("effect key: %s", effectFileName.c_str());
		m_dic.insert(make_pair(effectFileName, effectmodel));
	}
}


} //namespace ss
