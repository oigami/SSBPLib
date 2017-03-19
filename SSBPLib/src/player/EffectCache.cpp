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
	releseReference();
}


/**
* エフェクトファイル名を指定してEffectRefを得る
*/
SsEffectModel* EffectCache::getReference(const std::string& name)
{
	SsEffectModel* ref = _dic.at(name);
	return ref;
}

void EffectCache::dump()
{
	std::map<std::string, SsEffectModel*>::iterator it = _dic.begin();
	while(it != _dic.end())
	{
		SS_LOG("%s", (*it).second);
		++it;
	}
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
				switch(type)
				{
				case SsEffectFunctionType::Basic:
				{
					//基本情報
					ParticleElementBasic *effectParam = new ParticleElementBasic();
					effectParam->readData(reader);
					
					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::RndSeedChange:
				{
					//シード上書き
					ParticleElementRndSeedChange *effectParam = new ParticleElementRndSeedChange();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::Delay:
				{
					//発生：タイミング
					ParticleElementDelay *effectParam = new ParticleElementDelay();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::Gravity:
				{
					//重力を加える
					ParticleElementGravity *effectParam = new ParticleElementGravity();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::Position:
				{
					//座標：生成時
					ParticleElementPosition *effectParam = new ParticleElementPosition();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::Rotation:
				{
					//Z回転を追加
					ParticleElementRotation *effectParam = new ParticleElementRotation();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::TransRotation:
				{
					//Z回転速度変更
					ParticleElementRotationTrans *effectParam = new ParticleElementRotationTrans();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::TransSpeed:
				{
					//速度：変化
					ParticleElementTransSpeed *effectParam = new ParticleElementTransSpeed();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::TangentialAcceleration:
				{
					//接線加速度
					ParticleElementTangentialAcceleration *effectParam = new ParticleElementTangentialAcceleration();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::InitColor:
				{
					//カラーRGBA：生成時
					ParticleElementInitColor *effectParam = new ParticleElementInitColor();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::TransColor:
				{
					//カラーRGB：変化
					ParticleElementTransColor *effectParam = new ParticleElementTransColor();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::AlphaFade:
				{
					//フェード
					ParticleElementAlphaFade *effectParam = new ParticleElementAlphaFade();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::Size:
				{
					//スケール：生成時
					ParticleElementSize *effectParam = new ParticleElementSize();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::TransSize:
				{
					//スケール：変化
					ParticleElementTransSize *effectParam = new ParticleElementTransSize();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::PointGravity:
				{
					//重力点の追加
					ParticlePointGravity *effectParam = new ParticlePointGravity();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::TurnToDirectionEnabled:
				{
					//進行方向に向ける
					ParticleTurnToDirectionEnabled *effectParam = new ParticleTurnToDirectionEnabled();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				case SsEffectFunctionType::InfiniteEmitEnabled:
				{
					ParticleInfiniteEmitEnabled *effectParam = new ParticleInfiniteEmitEnabled();
					effectParam->readData(reader);

					behavior.plist.push_back(effectParam);												//パラメータを追加
					break;
				}
				default:
				break;
				}
			}

			SsEffectNode *node = new SsEffectNode(
				effectNode->parentIndex,
				(SsEffectNodeType::_enum)effectNode->type,
				behavior
			);
			effectmodel->nodeList.push_back(node);
			if(nodeindex == 0)
			{
			}
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

//エフェクトファイル情報の削除
void EffectCache::releseReference(void)
{
	std::map<std::string, SsEffectModel*>::iterator it = _dic.begin();
	while(it != _dic.end())
	{
		SsEffectModel* effectmodel = it->second;
		delete effectmodel;
		it++;
	}
	_dic.clear();
}


} //namespace ss
