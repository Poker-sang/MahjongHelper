#pragma once
#include "Block.h"

namespace MahjongHelper
{
    inline int BoolToInt(const bool _bool) { return _bool ? 1 : 0; }

    /// <summary>
    /// �����жϣ�������ǰ�жϣ�
    /// </summary>
    /// <returns>������</returns>
    inline List<Tile^>^ Opponent::ReadyHandJudge()
    {
        auto readyHands = new List<Tile^>();

        //���û�и�¶�����������жϣ�
        if (Melds == 0)
        {
            auto readyHandsList = ThirteenOrphansJudge();
            if (readyHandsList->Count != 0)
                return readyHandsList;
        }

        auto errBlocks = GetBlocks();

        //�������Ϳ���
        switch (errBlocks->Count)
        {
            //��һ�鲻�����ͣ�һ��ȸ�治�����ͣ�3n+1����
            //�����ڻ����������
        case 1:
        {
            //���˲������ͱ���
            readyHands->AddRange(errBlocks[0]->Traversal(Hands, true));
            break;
        }
        //�����鲻�����ͣ�һ�����Ӳ������ͣ�3n+2����һ��ȸͷ�����ͣ�3n+2����
        //�����鲻�����ͣ�һ�����Ӳ������ͣ�3n+2����һ��ȸͷ�����ͣ�3n+2����
        case 2:
        {
            if (errBlocks[1]->IgnoreEyesJudge(Hands))
                readyHands->AddRange(errBlocks[0]->Traversal(Hands, false));
            if (errBlocks[0]->IgnoreEyesJudge(Hands))
                readyHands->AddRange(errBlocks[1]->Traversal(Hands, false));
            break;
        }
        //�����鲻�����ͣ�����벻�����ͣ�3n+1����һ��ȸͷ�����ͣ�3n+2����
        case 3:
        {
            //���3n+2�Ĳ������ͼ����м����ȸͷ�����ͣ�����
            auto eyesIndex = 0;
            for each (auto eyesBlock in errBlocks)
                if (eyesBlock->Integrity == Block::EIntegrityType::Type2)
                    eyesIndex = errBlocks->IndexOf(eyesBlock);
            if (eyesIndex == 1 || !errBlocks[eyesIndex]->IgnoreEyesJudge(Hands))
                break;
            
            auto joint = JointBlocks(errBlocks[2 - eyesIndex],
                errBlocks[3 - eyesIndex]);
            if (joint == nullptr)
                break;
            //��������������������һ��
            if (joint->Item2->IntegrityJudge(joint->Item1, -1))
                readyHands->Add(joint->Item3);
            break;
        }
        //�����鲻�����ͣ�һ��ȸ�������ͣ�3n����һ��벻�����ͣ�3n+1����
        case 4:
        {
            auto joint = errBlocks[0]->FirstLoc < errBlocks[1]->FirstLoc ?
                JointBlocks(errBlocks[0], errBlocks[1]) :
                JointBlocks(errBlocks[1], errBlocks[0]);

            if (joint == nullptr)
                break;
            //�����������ȸͷ�����ͣ������һ��
            if (joint->Item2->IgnoreEyesJudge(joint->Item1))
                readyHands->Add(joint->Item3);

            break;
        }
        }
        return readyHands;
    }
    /// <summary>
    /// ��ʿ�����ж�
    /// </summary>
    /// <returns>����</returns>
    inline List<Tile^>^ Opponent::ThirteenOrphansJudge()
    {
        auto tempReturn = new List<Tile^>();
        //�Ƿ�ȱ��ĳ���۾��ƣ�0��1��
        auto shortage = false;
        //�Ƿ����ĳ���۾��ƣ�0��1��
        auto redundancy = false;
        auto shortTile = 0; //ȱ���۾���
        //�ж�ʮ�����۾��Ƶ�ӵ�����
        for (auto i = 0; i < 13; ++i)
        {
            const auto temp = (shortage ? 1 : 0) - (redundancy ? 1 : 0);
            //���������ӳ���۾���һ��
            if (Hands[i]->Val == (i + temp - 1) / 8)
            {
                //���֮ǰ�Ѿ���һ�������
                if (redundancy)
                    return tempReturn;
                redundancy = true; //��¼�ж���
            } //���������ӳ���۾���һ��
            else if (Hands[i]->Val == (i + temp + 1) / 8)
            {
                //���֮ǰ�Ѿ���һ��ȱ�����ǹ�ʿ�������¼ȱ��
                if (shortage)
                    return tempReturn;
                shortage = true;
                shortTile = i / 8;
            } //�в����۾��Ƽ������Ϲ�ʿ
            else if (Hands[i]->Val != (i + temp) / 8)
                return tempReturn;
        }
        //���ж��ţ�����һ������һ�棨���У�����Ϊ��������󲻻ᱻredundancy��¼��
        if (redundancy)
            tempReturn->Add(new Tile(shortage ? shortTile : 96));
        //����ȱ�������ʮ����
        else for (auto i = 0; i < 13; ++i)
            tempReturn->Add(new Tile(i / 8));
        return tempReturn;
    }

    /// <summary>
    /// ��ȡ�ֿ�
    /// </summary>
    /// <returns>�������Ŀ��������3����</returns>
    inline List<Block^>^ Opponent::GetBlocks()
    {
        auto errBlocks = new List<Block^>();
        auto blocks = new List<Block^>();
        blocks->Add(new Block(0));
        for (auto i = 0; i < Hands->Count - 1; ++i)
            //����ϵ������ͬ������
            if (GetRelation(Hands, i) > 1)
            {
                //��¼��һ��ĳ���
                Last(blocks)->Len = i - Last(blocks)->FirstLoc + 1;
                //ɸѡ������Lv->1
                switch (Last(blocks)->Len % 3)
                {
                case 0: Last(blocks)->Integrity = Block::EIntegrityType::Type0; break;
                case 1: Last(blocks)->Integrity = Block::EIntegrityType::Type1; break;
                case 2: Last(blocks)->Integrity = Block::EIntegrityType::Type2; break;
                default:throw new System::ArgumentOutOfRangeException();
                }
                //��������ǲ��������¼
                if (Last(blocks)->Integrity != Block::EIntegrityType::Type0)
                    errBlocks->Add(Last(blocks));
                //������Ŵﵽ(6 - ��¶��)����4����������������
                if (blocks->Count + Melds == 6 || errBlocks->Count == 4)
                    return new List<Block^>();
                //��һ�飬�������ǿ��������Ƶ����
                blocks->Add(new Block(i + 1));
            }
        //���һ��ļ�¼�޷�д��ѭ��
        {
            Last(blocks)->Len = Hands->Count - Last(blocks)->FirstLoc;
            switch (Last(blocks)->Len % 3)
            {
            case 0: Last(blocks)->Integrity = Block::EIntegrityType::Type0; break;
            case 1: Last(blocks)->Integrity = Block::EIntegrityType::Type1; break;
            case 2: Last(blocks)->Integrity = Block::EIntegrityType::Type2; break;
            default:throw new System::ArgumentOutOfRangeException();
            }
            if (Last(blocks)->Integrity != Block::EIntegrityType::Type0)
                errBlocks->Add(Last(blocks));
            if (errBlocks->Count == 4)
                return new List<Block^>();
        }
        //ͨ��������Lv.1�Ŀ飬ɸѡ������Lv.2������һ�鲻��������Ϊ�������ͼӰ벻�����ͣ�����һ��������
        for each (auto block in blocks)
            if (block->Integrity == Block::EIntegrityType::Type0 && !block->IntegrityJudge(Hands, -1))
            {
	            if (errBlocks->Count != 4)
	            {
	            	block->Integrity = Block::EIntegrityType::TypeEx;
	            	errBlocks->Add(block);
	            	//������
	            	errBlocks->Add(new Block(0));
	            	errBlocks->Add(new Block(0));
	            	errBlocks->Add(new Block(0));
	            }
	            else return new List<Block^>();
            }
        return errBlocks;
    }

    inline System::Tuple<List<Tile^>^, Block^, Tile^>^ Opponent::JointBlocks(Block^ frontBlock, Block^ followBlock)
    {
        //��ʱ��¼�м�����ƣ���������ƣ�
        auto tempReadyHands = new Tile(Hands[frontBlock->LastLoc()]->Val + 1);
        //���ԭ�����������м䲻�Ǹ�һ�ţ�������
        if (GetRelation(Hands, frontBlock->LastLoc()) != 2)
            return nullptr;
        //��ʱ�����жϵ�����
        auto jointedHands = new List<Tile^>();
        //�����鲻������������
        auto jointedBlock = new Block(0);
        jointedBlock->Len = frontBlock->Len + 1 + followBlock->Len;
        //���Ƹò�������������
        jointedHands->AddRange(Hands->GetRange(frontBlock->FirstLoc, jointedBlock->Len - 1));
        //����һ���м������
        jointedHands->Insert(frontBlock->Len, tempReadyHands);
        return new System::Tuple<List<Tile^>^, Block^, Tile^>(jointedHands, jointedBlock, tempReadyHands);
    }
}
