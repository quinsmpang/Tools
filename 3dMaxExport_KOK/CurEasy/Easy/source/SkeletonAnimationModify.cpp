#include "SkeletonAnimationModify.h"
#include "UtilWrap.h"

namespace EasyOgreExporter
{
	DEFINE_SET_GET_RETPFUNC(SkeletonAnimationModify, std::vector<Modify_ClipFrameItem*>, clipFrameItemVec);

	SkeletonAnimationModify::SkeletonAnimationModify(IGameModifier* pGameModifier)
	{
		m_pGameModifie = pGameModifier;
		initModifyData();
	}

	SkeletonAnimationModify::~SkeletonAnimationModify()
	{

	}

	void SkeletonAnimationModify::cleanClipItemVec()
	{
		int idx = 0;
		for (idx = 0; idx < m_clipFrameItemVec.size(); ++idx)
		{
			delete m_clipFrameItemVec[idx];
		}
	}

	void SkeletonAnimationModify::initModifyData()
	{
		cleanClipItemVec();

		IPropertyContainer* pIPropertyContainer;
		IGameProperty* pIGameProperty;
		IParamBlock2* pIParamBlock2;

		pIPropertyContainer = m_pGameModifie->GetIPropertyContainer();	// ���в�������
		int numofProps = pIPropertyContainer->GetNumberOfProperties();	// ������������
		// Modify ������ �� 0 �����ܹ��� 1 ��
		int idx = 0;
		for (idx = 0; idx < numofProps; ++idx)
		{
			pIGameProperty = pIPropertyContainer->GetProperty(0);	// ������������
			if (pIGameProperty->IsPBlock2())
			{
				pIParamBlock2 = pIGameProperty->GetMaxParamBlock2();
				ReadAnimationClipsBlock(pIParamBlock2);
			}
		}
	}

	void SkeletonAnimationModify::ReadAnimationClipsBlock(IParamBlock2 *pb)
	{
		int numBlockparams = pb->NumParams();
		int p = 0;
		ParamID pid_names = NULL;
		ParamID pid_starts = NULL;
		ParamID pid_ends = NULL;
		int minCount = 100000;
		for (p = 0; p < numBlockparams; p++) 
		{
			ParamID pid = pb->IndextoID(p);
			ParamDef def = pb->GetParamDef(pid);
			char * paramName = UtilWrap::W2A(def.int_name);
			if (UtilWrap::isStrEqual(paramName, "anames"))
			{
				pid_names = pid;
				if (pb->Count(pid) < minCount)
				{ 
					minCount = pb->Count(pid); 
				}
			}
			if (UtilWrap::isStrEqual(paramName, "afframes"))
			{
				pid_starts = pid;
				if (pb->Count(pid) < minCount)
				{ 
					minCount = pb->Count(pid); 
				}
			}
			if (UtilWrap::isStrEqual(paramName, "alframes"))
			{
				pid_ends = pid;
				if (pb->Count(pid) < minCount)
				{ 
					minCount = pb->Count(pid); 
				}
			}
			free(paramName);
		}
		if ((pid_names == NULL) || (pid_starts == NULL) || (pid_ends == NULL)){
			minCount = 0;
		}
		if (minCount != 0)
		{
			int clipCnt = 0;
			for (clipCnt = 0; clipCnt < minCount; clipCnt++)
			{
				char * thisName_ptr = UtilWrap::W2A(pb->GetStr(pid_names, 0, clipCnt));
				int startFrame = pb->GetInt(pid_starts, 0, clipCnt);
				int endFrame = pb->GetInt(pid_ends, 0, clipCnt);
				//checking if name is unique
				Modify_ClipFrameItem * clipItem = new Modify_ClipFrameItem;
				m_clipFrameItemVec.push_back(clipItem);
				clipItem->m_name = thisName_ptr;
				clipItem->m_startFrame = startFrame * GetTicksPerFrame();		// ������� tick ������ Frame ������ʹ�õĶ��� tick
				clipItem->m_endFrame = endFrame * GetTicksPerFrame();

				free(thisName_ptr);
			}
		}
		pid_names = NULL;
		pid_starts = NULL;
		pid_ends = NULL;
	}
}