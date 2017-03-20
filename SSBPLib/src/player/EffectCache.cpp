#include "EffectCache.h"
#include <string>
#include <map>
#include "DataArrayReader.h"
#include "player/Util.h"
#include "player/ToPointer.h"
#include "player/CellCache.h"
#include "effect/ssloader_ssee.h"

namespace ss{


EffectCache::EffectCache(const ProjectData* data, const std::string& imageBaseDir, CellCache* cellCache)
{
	init(data, imageBaseDir, cellCache);
}
EffectCache::~EffectCache()
{
	for(auto str_model : _dic){
		SS_SAFE_DELETE(str_model.second);
	}
	_dic.clear();
}


/**
* エフェクトファイル名を指定してEffectRefを得る
*/
const SsEffectModel* EffectCache::getReference(const std::string& name) const
{
	const SsEffectModel* ref = _dic.at(name);
	return ref;
}


void EffectCache::init(const ProjectData* data, const std::string& imageBaseDir, CellCache* cellCache)
{
	SS_ASSERT_LOG(data != NULL, "Invalid data");

	ToPointer ptr(data);

	//ssbpからエフェクトファイル配列を取得
	const EffectFile* effectFileArray = ptr.toEffectFiles(data);

	for(int listindex = 0; listindex < data->numEffectFileList; listindex++)
	{
		//エフェクトファイル配列からエフェクトファイルを取得
		const EffectFile* effectFile = &effectFileArray[listindex];
		std::string effectFileName = ptr.toString(effectFile->name);

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

			//セル情報を作成
			int cellIndex = effectNode->cellIndex;
			const CellRef* cellRef = cellIndex >= 0 ? cellCache->getReference(cellIndex) : nullptr;
			SsRenderBlendType blendType = static_cast<SsRenderBlendType>(effectNode->blendType);

			SsEffectNode *node = new SsEffectNode(
				effectNode->parentIndex,
				static_cast<SsEffectNodeType>(effectNode->type),
				cellIndex, cellRef, blendType
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

		SS_LOG("effect key: %s", effectFileName.c_str());
		_dic.insert(std::map<std::string, SsEffectModel*>::value_type(effectFileName, effectmodel));
	}
}


} //namespace ss
