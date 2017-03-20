#include "EffectCache.h"
#include <string>
#include <map>
#include "DataArrayReader.h"
#include "player/Util.h"
#include "player/ToPointer.h"
#include "player/CellCache.h"
#include "common/Loader/ssloader_ssee.h"

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

		//保持用のエフェクトファイル情報を作成
		SsEffectModel *effectmodel = new SsEffectModel();
		std::string effectFileName = ptr.toString(effectFile->name);

		//エフェクトファイルからエフェクトノード配列を取得
		const EffectNode* effectNodeArray = ptr.toEffectNodes(effectFile);
		for(int nodeindex = 0; nodeindex < effectFile->numNodeList; nodeindex++)
		{
			const EffectNode* effectNode = &effectNodeArray[nodeindex];		//エフェクトノード配列からエフェクトノードを取得

			//セル情報を作成
			int cellIndex = effectNode->cellIndex;
			const CellRef* cellRef = cellIndex >= 0 ? cellCache->getReference(cellIndex) : nullptr;
			SsRenderBlendType::_enum blendType = static_cast<SsRenderBlendType::_enum>(effectNode->blendType);

			SsEffectBehavior behavior(cellIndex, cellRef, blendType);
			

			//エフェクトノードからビヘイビア配列を取得
			const ss_offset* behaviorArray = static_cast<const ss_offset*>(ptr(effectNode->Behavior));
			for(int behaviorindex = 0; behaviorindex < effectNode->numBehavior; behaviorindex++)
			{
				//ビヘイビア配列からビヘイビアを取得
				const ss_u16* behavior_adr = static_cast<const ss_u16*>(ptr(behaviorArray[behaviorindex]));
				DataArrayReader reader(behavior_adr);

				//パラメータを作ってpush_backで登録していく
				int type = reader.readS32();
				SsEffectElementBase* effectParam = SsEffectElementBase::create( static_cast<SsEffectFunctionType::enum_>(type) );
				if(effectParam){
					effectParam->readData(reader);			//データ読み込み
					behavior.plist.push_back(effectParam);
				}
			}

			SsEffectNode *node = new SsEffectNode(
				effectNode->parentIndex,
				(SsEffectNodeType::_enum)effectNode->type,
				behavior
			);
			effectmodel->nodeList.push_back(node);
		}
		effectmodel->lockRandSeed = effectFile->lockRandSeed; 	 // ランダムシード固定値
		effectmodel->isLockRandSeed = effectFile->isLockRandSeed;  // ランダムシードを固定するか否か
		effectmodel->fps = effectFile->fps;             //
		effectmodel->effectName = effectFileName;
		effectmodel->layoutScaleX = effectFile->layoutScaleX;	//レイアウトスケールX
		effectmodel->layoutScaleY = effectFile->layoutScaleY;	//レイアウトスケールY



		SS_LOG("effect key: %s", effectFileName.c_str());
		_dic.insert(std::map<std::string, SsEffectModel*>::value_type(effectFileName, effectmodel));
	}
}


} //namespace ss
